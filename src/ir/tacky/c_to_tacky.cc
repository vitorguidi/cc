#include "src/ir/tacky/c_to_tacky.h"
#include "src/ast/tacky.h"
#include <memory>
#include <algorithm>

namespace IR {

template<typename T>
std::shared_ptr<T> AstToTackyVisitor::get_result()
{
    if (result_buffer_.empty()) {
        throw std::runtime_error("Result buffer is empty when fetching result");
    }
    auto ast_node = result_buffer_.back();
    result_buffer_.pop_back();
    auto casted_node = std::dynamic_pointer_cast<T>(ast_node);
    if (!casted_node) {
        throw std::runtime_error("Failed to cast AST node in result buffer");
    }
    return casted_node;
}

template <typename T>
std::vector<std::shared_ptr<T>> AstToTackyVisitor::get_instructions() {
    std::vector<std::shared_ptr<T>> results;
    while (!instruction_buffer.empty()) {
        auto back_node = instruction_buffer.back();
        instruction_buffer.pop_back();
        auto casted_node = std::dynamic_pointer_cast<T>(back_node);
        if (!casted_node) {
            throw std::runtime_error("Failed to cast AST node in instruction buffer");
        }
        results.push_back(casted_node);
    }
    std::reverse(results.begin(), results.end());
    return results;
}

template<std::derived_from<Tacky::BinaryOpNode> T>
void AstToTackyVisitor::visit_bin_exp(CAst::BinaryExpressionNode& node) {
    node.left_->accept(*this);
    auto left_expression_result = get_result<Tacky::ValueNode>();
    node.right_->accept(*this);
    auto right_expression_result = get_result<Tacky::ValueNode>();
    auto dst = generate_temp_var_name();

    auto dst_node = std::make_shared<Tacky::VariableNode>(dst);

    auto tacky_op = std::make_shared<T>(
        left_expression_result,
        right_expression_result,
        dst_node
    );

    instruction_buffer.push_back(tacky_op);
    result_buffer_.push_back(dst_node);    
}

enum LoopLabelSuffix {
    START,
    CONTINUE,
    BREAK,
};

std::string loop_label(std::string label_prefix, LoopLabelSuffix suf) {
    switch (suf) {
        case START:
            return label_prefix + "_start_";
        case CONTINUE:
            return label_prefix + "_continue_";
        case BREAK:
            return label_prefix + "_break_";
        default:
            throw std::runtime_error("Unknown suffix.");
    }
}

template<std::derived_from<Tacky::UnaryNode> T>
void AstToTackyVisitor::visit_un_exp(CAst::UnaryExpressionNode& node) {

    node.operand_->accept(*this);

    auto inner_expression_result = get_result<Tacky::ValueNode>();
    auto dst = generate_temp_var_name();

    auto tacky_op = std::make_shared<T>(
        inner_expression_result,
        std::make_shared<Tacky::VariableNode>(dst)
    );

    auto value_return_hint = std::make_shared<Tacky::VariableNode>(dst);
    
    instruction_buffer.push_back(tacky_op);
    result_buffer_.push_back(value_return_hint);
}

std::shared_ptr<Tacky::ProgramNode> AstToTackyVisitor::get_tacky_from_c_ast(std::shared_ptr<CAst::ProgramNode> root_node)
{
    if (!instruction_buffer.empty() || !result_buffer_.empty()) {
        throw std::runtime_error("Buffers not empty at start of conversion");
    }
    root_node->accept(*this);
    if (result_buffer_.size() != 1) {
        throw std::runtime_error("Expected exactly one Tacky AST root node");
    }
    auto result_node = std::dynamic_pointer_cast<Tacky::ProgramNode>(result_buffer_.back());
    result_buffer_.pop_back();
    if (!result_node) {
        throw std::runtime_error("Expected Tacky ProgramNode as conversion result");
    }
    if (!instruction_buffer.empty() || !result_buffer_.empty()) {
        throw std::runtime_error("Buffers not empty at end of conversion");
    }
    return result_node;
}

void AstToTackyVisitor::visit(CAst::ProgramNode& node) {
    std::vector<std::shared_ptr<Tacky::FunctionNode>> processed_funcs;
    for (auto& function : node.functions_) {
        function->accept(*this);
        auto fn =get_result<Tacky::FunctionNode>();
        processed_funcs.push_back(fn);
    }
    auto result = std::make_shared<Tacky::ProgramNode>(std::move(processed_funcs) );
    result_buffer_.push_back(result);
}

void AstToTackyVisitor::visit(CAst::FunctionNode& node) {

    std::vector<std::shared_ptr<Tacky::InstructionNode>> tacky_instructions;

    if (node.body_.has_value()) {
        node.body_.value()->accept(*this);
        auto result = get_result<Tacky::ValueNode>();
        tacky_instructions = get_instructions<Tacky::InstructionNode>();
    }

    std::vector<std::string> args;
    for(auto arg : node.arguments_node_->arguments_) {
        args.push_back(arg.name);
    }

    auto function_result = std::make_shared<Tacky::FunctionNode>(
        node.name_,
        tacky_instructions,
        std::move(args)
    );

    if (!tacky_instructions.empty()) {
        result_buffer_.push_back(function_result);
        return;
    }

    result_buffer_.push_back(std::make_shared<Tacky::NullNode>());
}

void AstToTackyVisitor::visit(CAst::FunctionCallNode& node) {

    std::vector<std::shared_ptr<Tacky::ValueNode>> processed_args;
    for(auto arg : node.args_) {
        arg->accept(*this);
        processed_args.push_back(get_result<Tacky::ValueNode>());
    }

    auto dst = std::make_shared<Tacky::VariableNode>(generate_temp_var_name());

    instruction_buffer.push_back(
        std::make_shared<Tacky::FunctionCallNode>(
            node.name_,
            std::move(processed_args),
            dst
        )
    );

    result_buffer_.push_back(dst);
    
}

void AstToTackyVisitor::visit(CAst::ReturnStatementNode& node) {
    if (node.type_ != CAst::Type::INTEGER) {
       throw std::runtime_error("Only integer return types are supported for now");
    }

    // recursively parses an expression and returns a value node
    node.return_value_->accept(*this);

    // if we end up on a leaf value node, we can just create a return node
    // leaf visitors push the value onto the stack

    instruction_buffer.push_back(
        std::make_shared<Tacky::ReturnNode>(get_result<Tacky::ValueNode>())
    );

    result_buffer_.push_back(std::make_shared<Tacky::NullNode>());
}

// unary expressions
void AstToTackyVisitor::visit(CAst::BitwiseNotUnaryExpressionNode& node) {visit_un_exp<Tacky::BitwiseNotNode>(node);}
void AstToTackyVisitor::visit(CAst::MinusUnaryExpressionNode& node) {visit_un_exp<Tacky::ComplementNode>(node);}
void AstToTackyVisitor::visit(CAst::NotUnaryExpressionNode& node) {visit_un_exp<Tacky::NotNode>(node);}

// unary arithmetic expressions
void AstToTackyVisitor::visit(CAst::DivNode& node) {visit_bin_exp<Tacky::DivNode>(node);}
void AstToTackyVisitor::visit(CAst::MultNode& node) {visit_bin_exp<Tacky::MultNode>(node);}
void AstToTackyVisitor::visit(CAst::ModNode& node) {visit_bin_exp<Tacky::ModNode>(node);}
void AstToTackyVisitor::visit(CAst::MinusNode& node) {visit_bin_exp<Tacky::MinusNode>(node);}
void AstToTackyVisitor::visit(CAst::PlusNode& node) {visit_bin_exp<Tacky::PlusNode>(node);}

// relational binary expressions
void AstToTackyVisitor::visit(CAst::EqualNode& node) {visit_bin_exp<Tacky::EqualNode>(node);}
void AstToTackyVisitor::visit(CAst::NotEqualNode& node) {visit_bin_exp<Tacky::NotEqualNode>(node);}
void AstToTackyVisitor::visit(CAst::GreaterNode& node) {visit_bin_exp<Tacky::GreaterNode>(node);}
void AstToTackyVisitor::visit(CAst::GreaterEqNode& node) {visit_bin_exp<Tacky::GreaterEqNode>(node);}
void AstToTackyVisitor::visit(CAst::LessNode& node) {visit_bin_exp<Tacky::LessNode>(node);}
void AstToTackyVisitor::visit(CAst::LessEqNode& node) {visit_bin_exp<Tacky::LessEqNode>(node);}

// unary boolean expressions
void AstToTackyVisitor::visit(CAst::BitwiseAndNode& node) {visit_bin_exp<Tacky::BitwiseAndNode>(node);}
void AstToTackyVisitor::visit(CAst::BitwiseOrNode& node) {visit_bin_exp<Tacky::BitwiseOrNode>(node);}
void AstToTackyVisitor::visit(CAst::BitwiseLeftShiftNode& node) {visit_bin_exp<Tacky::BitwiseLeftShiftNode>(node);}
void AstToTackyVisitor::visit(CAst::BitwiseRightShiftNode& node) {visit_bin_exp<Tacky::BitwiseRightShiftNode>(node);}
void AstToTackyVisitor::visit(CAst::BitwiseXorNode& node) {visit_bin_exp<Tacky::BitwiseXorNode>(node);}

// short circuiting bin exps

void AstToTackyVisitor::visit(CAst::AndNode& node) {
    
//     x = eval e1
//     jiz  x false
//     y = eval e2
//     jiz y false
//     dst = true
//     jmp fi
// false
//     mv 0 dst
// end

    auto base_label = generate_temp_label();
    auto false_label = std::make_shared<Tacky::LabelNode>(base_label + "false_");
    auto end_label = std::make_shared<Tacky::LabelNode>(base_label + "end_");

    auto dst = std::make_shared<Tacky::VariableNode>(generate_temp_var_name());

    node.left_->accept(*this);
    auto left_expression_result = get_result<Tacky::ValueNode>();

    instruction_buffer.push_back(std::make_shared<Tacky::JumpIfZeroNode>(
        left_expression_result,
        false_label
    ));

    node.right_->accept(*this);
    auto right_expression_result = get_result<Tacky::ValueNode>();

    instruction_buffer.push_back(std::make_shared<Tacky::JumpIfZeroNode>(
        right_expression_result,
        false_label
    ));

    instruction_buffer.push_back(std::make_shared<Tacky::MovNode>(
        std::make_shared<Tacky::IntegerNode>(1),
        dst
    ));

    instruction_buffer.push_back(std::make_shared<Tacky::JumpNode>(end_label));
    instruction_buffer.push_back(false_label);
    instruction_buffer.push_back(std::make_shared<Tacky::MovNode>(
        std::make_shared<Tacky::IntegerNode>(0),
        dst
    ));
    instruction_buffer.push_back(end_label);
    // dst as result of the whole ordeal
    result_buffer_.push_back(dst);
}

void AstToTackyVisitor::visit(CAst::OrNode& node) {
//     x = eval e1
//     jiz  x false
//     y = eval e2
//     jiz y false
//     dst = true
//     jmp fi
// false
//     mv 0 dst
// end
    auto base_label = generate_temp_label();
    auto true_label = std::make_shared<Tacky::LabelNode>(base_label + "true_");
    auto end_label = std::make_shared<Tacky::LabelNode>(base_label + "end_");

    auto dst = std::make_shared<Tacky::VariableNode>(generate_temp_var_name());

    node.left_->accept(*this);
    auto left_expression_result = get_result<Tacky::ValueNode>();

    instruction_buffer.push_back(std::make_shared<Tacky::JumpIfNotZeroNode>(
        left_expression_result,
        true_label
    ));

    node.right_->accept(*this);
    auto right_expression_result = get_result<Tacky::ValueNode>();

    instruction_buffer.push_back(std::make_shared<Tacky::JumpIfNotZeroNode>(
        right_expression_result,
        true_label
    ));

    instruction_buffer.push_back(std::make_shared<Tacky::MovNode>(
        std::make_shared<Tacky::IntegerNode>(0),
        dst
    ));

    instruction_buffer.push_back(std::make_shared<Tacky::JumpNode>(end_label));
    instruction_buffer.push_back(true_label);
    instruction_buffer.push_back(std::make_shared<Tacky::MovNode>(
        std::make_shared<Tacky::IntegerNode>(1),
        dst
    ));
    instruction_buffer.push_back(end_label);
    // dst as result of the whole ordeal
    result_buffer_.push_back(dst);
}

void AstToTackyVisitor::visit(CAst::IntegerValueNode& node) {
    result_buffer_.push_back(
        std::make_shared<Tacky::IntegerNode>(node.value_)
    );
}

void AstToTackyVisitor::visit(CAst::TypeNode& node) {}

void AstToTackyVisitor::visit(CAst::FunctionArgumentsNode& node) {}

void AstToTackyVisitor::visit(CAst::BlockNode& node) {
    for (auto& statement : node.statements_) {
        statement->accept(*this);
        // Forcefully remove the result of expression statements
        auto result = get_result<Tacky::ValueNode>(); 
    }
    result_buffer_.push_back(std::make_shared<Tacky::NullNode>());
}

void AstToTackyVisitor::visit(CAst::AssignmentNode& node) {
    node.right_->accept(*this);
    auto right_expression_result = get_result<Tacky::ValueNode>();
    auto original_var = std::dynamic_pointer_cast<CAst::VariableNode>(node.left_);
    if (!original_var) {
        throw std::runtime_error("Expected left operand in CAst assignment to be a variable node.");
    }
    original_var->accept(*this);
    auto casted_var = get_result<Tacky::VariableNode>();
    instruction_buffer.push_back(std::make_shared<Tacky::MovNode>(
        right_expression_result,
        casted_var
    ));
    result_buffer_.push_back(casted_var);
}

void AstToTackyVisitor::visit(CAst::VariableNode& node) {
    result_buffer_.push_back(std::make_shared<Tacky::VariableNode>(node.name_));
}

void AstToTackyVisitor::visit(CAst::NullNode& node) {
    result_buffer_.push_back(std::make_shared<Tacky::NullNode>());
}

void AstToTackyVisitor::visit(CAst::VariableDeclarationNode& node) {
    if (node.expr_) {
        node.expr_.value()->accept(*this);
        auto val = get_result<Tacky::ValueNode>();
        instruction_buffer.push_back(std::make_shared<Tacky::MovNode>(
            val, 
            std::make_shared<Tacky::VariableNode>(node.var_->name_)
        ));
    }
    result_buffer_.push_back(std::make_shared<Tacky::NullNode>());

}

void AstToTackyVisitor::visit(CAst::IfNode& node) {
    node.cond_->accept(*this);
    auto cond_expr_result = get_result<Tacky::ValueNode>();
    auto label = generate_temp_label();
    auto else_label = label + "_else_";
    auto end_label = label + "_end_";
    instruction_buffer.push_back(std::make_shared<Tacky::JumpIfZeroNode>(
        cond_expr_result,
        std::make_shared<Tacky::LabelNode>(else_label)
    ));
    node.then_->accept(*this);
    result_buffer_.pop_back();
    instruction_buffer.push_back(std::make_shared<Tacky::JumpNode>(
        std::make_shared<Tacky::LabelNode>(end_label)
    ));
    instruction_buffer.push_back(std::make_shared<Tacky::LabelNode>(else_label));
    if (node.alt_) {
        node.alt_.value()->accept(*this);
        result_buffer_.pop_back();
    }
    instruction_buffer.push_back(std::make_shared<Tacky::LabelNode>(end_label));
    result_buffer_.push_back(std::make_shared<Tacky::NullNode>());
}

void AstToTackyVisitor::visit(CAst::TernaryNode& node) {
    node.cond_->accept(*this);
    auto cond_expr_result = get_result<Tacky::ValueNode>();
    auto label = generate_temp_label();
    auto else_label = label + "_else_";
    auto end_label = label + "_end_";
    auto result_var = std::make_shared<Tacky::VariableNode>(generate_temp_var_name());
    instruction_buffer.push_back(std::make_shared<Tacky::JumpIfZeroNode>(
        cond_expr_result,
        std::make_shared<Tacky::LabelNode>(else_label)
    ));
    node.then_->accept(*this);
    auto then_result = get_result<Tacky::ValueNode>();
    instruction_buffer.push_back(std::make_shared<Tacky::MovNode>(
        then_result,
        result_var
    ));
    instruction_buffer.push_back(std::make_shared<Tacky::JumpNode>(
        std::make_shared<Tacky::LabelNode>(end_label)
    ));
    instruction_buffer.push_back(std::make_shared<Tacky::LabelNode>(else_label));
    node.alt_->accept(*this);
    auto alt_result = get_result<Tacky::ValueNode>();
    instruction_buffer.push_back(std::make_shared<Tacky::MovNode>(
        alt_result,
        result_var
    ));
    instruction_buffer.push_back(std::make_shared<Tacky::LabelNode>(end_label));
    result_buffer_.push_back(result_var);
}

void AstToTackyVisitor::visit(CAst::ForNode& node) {
    auto start_label = std::make_shared<Tacky::LabelNode>(
        loop_label(node.label_, LoopLabelSuffix::START));

    auto continue_label = std::make_shared<Tacky::LabelNode>(
        loop_label(node.label_, LoopLabelSuffix::CONTINUE)
    );
    
    auto break_label = std::make_shared<Tacky::LabelNode>(
        loop_label(node.label_, LoopLabelSuffix::BREAK)
    );

    node.init_->accept(*this);
    auto init_result = get_result<Tacky::ValueNode>();

    instruction_buffer.push_back(start_label);
    if (node.cond_) {
        node.cond_.value()->accept(*this);
        auto cond_result = get_result<Tacky::ValueNode>();
        instruction_buffer.push_back(
            std::make_shared<Tacky::JumpIfZeroNode>(
                cond_result,
                break_label
            )
        );
    }

    node.body_->accept(*this);
    auto body_result = get_result<Tacky::ValueNode>();

    instruction_buffer.push_back(continue_label);

    if(node.post_) {
        node.post_.value()->accept(*this);
        auto post_result = get_result<Tacky::ValueNode>();
    }

    instruction_buffer.push_back(
        std::make_shared<Tacky::JumpNode>(
            start_label
        )
    );

    instruction_buffer.push_back(break_label);

    result_buffer_.push_back(std::make_shared<Tacky::NullNode>());

}

void AstToTackyVisitor::visit(CAst::WhileNode& node) {
    auto continue_label = std::make_shared<Tacky::LabelNode>(
        loop_label(node.label_, LoopLabelSuffix::CONTINUE)
    );
    
    auto break_label = std::make_shared<Tacky::LabelNode>(
        loop_label(node.label_, LoopLabelSuffix::BREAK)
    );

    instruction_buffer.push_back(continue_label);

    node.cond_->accept(*this);
    auto cond_result = get_result<Tacky::ValueNode>();

    instruction_buffer.push_back(std::make_shared<Tacky::JumpIfZeroNode>(
        cond_result,
        break_label
    ));

    node.body_->accept(*this);
    auto body_result = get_result<Tacky::ValueNode>();

    instruction_buffer.push_back(std::make_shared<Tacky::JumpNode>(continue_label));
    instruction_buffer.push_back(break_label);

    result_buffer_.push_back(std::make_shared<Tacky::NullNode>());

}

void AstToTackyVisitor::visit(CAst::DoWhileNode& node) {
    auto start_label = std::make_shared<Tacky::LabelNode>(
        loop_label(node.label_, LoopLabelSuffix::START));
    
    auto continue_label = std::make_shared<Tacky::LabelNode>(
        loop_label(node.label_, LoopLabelSuffix::CONTINUE)
    );
    
    auto break_label = std::make_shared<Tacky::LabelNode>(
        loop_label(node.label_, LoopLabelSuffix::BREAK)
    );

    instruction_buffer.push_back(start_label);

    node.body_->accept(*this);
    // do not pop a result, since statements do not return values
    auto body_result = get_result<Tacky::ValueNode>();

    instruction_buffer.push_back(continue_label);

    node.cond_->accept(*this);
    auto cond_result = get_result<Tacky::ValueNode>();

    instruction_buffer.push_back(std::make_shared<Tacky::JumpIfNotZeroNode>(
        cond_result,
        start_label
    ));

    instruction_buffer.push_back(break_label);
    result_buffer_.push_back(std::make_shared<Tacky::NullNode>());

}

void AstToTackyVisitor::visit(CAst::ContinueNode& node) {
    auto continue_label = std::make_shared<Tacky::LabelNode>(loop_label(node.label_, LoopLabelSuffix::CONTINUE));
    instruction_buffer.push_back(
        std::make_shared<Tacky::JumpNode>(continue_label)
    );
    result_buffer_.push_back(std::make_shared<Tacky::NullNode>());
}

void AstToTackyVisitor::visit(CAst::BreakNode& node) {
    auto break_label = std::make_shared<Tacky::LabelNode>(loop_label(node.label_, LoopLabelSuffix::BREAK));
    instruction_buffer.push_back(
        std::make_shared<Tacky::JumpNode>(break_label)
    );
    result_buffer_.push_back(std::make_shared<Tacky::NullNode>());
}

std::string AstToTackyVisitor::generate_temp_var_name() {
    return "_tacky_temp_" + std::to_string(temp_var_counter_++);
}

std::string AstToTackyVisitor::generate_temp_label() {
    return "_label_" + std::to_string(label_counter_++) + "_";
}

} // namespace IR