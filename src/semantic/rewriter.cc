#include "src/semantic/semantic.h"

namespace Semantic {


void SemanticCAstRewriter::visit(CAst::VariableDeclarationNode& node) {
    std::optional<std::shared_ptr<CAst::ExpressionNode>> converted_expr = std::nullopt;
    if (node.expr_) {
        node.expr_.value()->accept(*this);
        converted_expr = As<CAst::ExpressionNode>(buffer_.back());
        buffer_.pop_back();
    }
    buffer_.push_back(std::make_shared<CAst::VariableDeclarationNode>(
        node.var_,
        node.type_,
        converted_expr
    ));
}

void SemanticCAstRewriter::visit(CAst::AssignmentNode& node) {
    node.left_->accept(*this);
    auto casted_left = As<CAst::ExpressionNode>(buffer_.back());
    buffer_.pop_back();
    auto left_as_var = std::dynamic_pointer_cast<CAst::VariableNode>(casted_left);
    if (!left_as_var) throw std::runtime_error("Can only assign to a variable, invalid lvalue");
    node.right_->accept(*this);
    auto casted_right = As<CAst::ExpressionNode>(buffer_.back());
    buffer_.pop_back();
    buffer_.push_back(std::make_shared<CAst::AssignmentNode>(
        casted_left,
        casted_right
    ));
}

void SemanticCAstRewriter::visit(CAst::VariableNode& node) {
    buffer_.push_back(std::make_shared<CAst::VariableNode>(node.name_));
}

void SemanticCAstRewriter::visit(CAst::ProgramNode& node) {
    is_on_file_scope = true;
    symbol_table_.emplace_back();
    std::vector<std::shared_ptr<CAst::FunctionNode>> functions;
    for (auto fn : node.functions_) {
        is_on_file_scope = false;
        fn->accept(*this);
        auto processed_fn = As<CAst::FunctionNode>(buffer_.back());
        functions.push_back(processed_fn);
        buffer_.pop_back();
        is_on_file_scope = true;
    }
    buffer_.push_back(std::make_shared<CAst::ProgramNode>(std::move(functions)));
    symbol_table_.pop_back();
}

void SemanticCAstRewriter::visit(CAst::FunctionNode& node) {
    node.arguments_node_->accept(*this);
    auto args = As<CAst::FunctionArgumentsNode>(buffer_.back());
    buffer_.pop_back();

    std::optional<std::shared_ptr<CAst::BlockNode>> body = std::nullopt;

    if (node.body_) {
        symbol_table_.emplace_back();
        node.body_.value()->accept(*this);

        body = std::make_optional(As<CAst::BlockNode>(buffer_.back()));
        buffer_.pop_back();
        symbol_table_.pop_back();
    }


    buffer_.push_back(std::make_shared<CAst::FunctionNode>(
        node.name_,
        node.type_node_,
        args,
        body
    ));
}

void SemanticCAstRewriter::visit(CAst::FunctionCallNode& node) {
    std::vector<std::shared_ptr<CAst::ExpressionNode>> processed_args;
    for(auto arg : node.args_) {
        arg->accept(*this);
        auto resolved_arg = As<CAst::ExpressionNode>(buffer_.back());
        buffer_.pop_back();
        processed_args.push_back(resolved_arg);
    }
    buffer_.push_back(std::make_shared<CAst::FunctionCallNode>(
        node.name_,
        std::move(processed_args)
    ));
}

void SemanticCAstRewriter::visit(CAst::TypeNode& node) {
    buffer_.push_back(std::make_shared<CAst::TypeNode>(node.type_));
}

void SemanticCAstRewriter::visit(CAst::FunctionArgumentsNode& node) {
    buffer_.push_back(std::make_shared<CAst::FunctionArgumentsNode>(std::move(node.arguments_)));
}

void SemanticCAstRewriter::visit(CAst::BlockNode& node) {
    symbol_table_.emplace_back();
    std::vector<std::shared_ptr<CAst::BlockElementNode>> els;
    for(auto& el : node.statements_) {
        el->accept(*this);
        auto processed_el = As<CAst::BlockElementNode>(buffer_.back());
        buffer_.pop_back();
        els.push_back(processed_el);
    }
    buffer_.push_back(std::make_shared<CAst::BlockNode>(
        std::move(els)
    ));
    symbol_table_.pop_back();
}

void SemanticCAstRewriter::visit(CAst::ReturnStatementNode& node) {
    node.return_value_->accept(*this);
    auto processed_expr = As<CAst::ExpressionNode>(buffer_.back());
    buffer_.pop_back();
    buffer_.push_back(std::make_shared<CAst::ReturnStatementNode>(node.type_, processed_expr));
}

void SemanticCAstRewriter::visit(CAst::NullNode& node) {buffer_.push_back(std::make_shared<CAst::NullNode>());}

    // unary exps

template<std::derived_from<CAst::UnaryExpressionNode> T>
void SemanticCAstRewriter::visit_unexp(CAst::UnaryExpressionNode& node) {
    node.operand_->accept(*this);
    auto processed_operand = As<CAst::ExpressionNode>(buffer_.back());
    buffer_.pop_back();
    buffer_.push_back(std::make_shared<T>(processed_operand));
}

void SemanticCAstRewriter::visit(CAst::BitwiseNotUnaryExpressionNode& node) {visit_unexp<CAst::BitwiseNotUnaryExpressionNode>(node);}
void SemanticCAstRewriter::visit(CAst::MinusUnaryExpressionNode& node) {visit_unexp<CAst::MinusUnaryExpressionNode>(node);}
void SemanticCAstRewriter::visit(CAst::NotUnaryExpressionNode& node) {visit_unexp<CAst::NotUnaryExpressionNode>(node);}


    // bin exps

template<std::derived_from<CAst::BinaryExpressionNode> T>
void SemanticCAstRewriter::visit_binexp(CAst::BinaryExpressionNode& node) {
    node.left_->accept(*this);
    auto processed_left = As<CAst::ExpressionNode>(buffer_.back());
    buffer_.pop_back();

    node.right_->accept(*this);
    auto processed_right = As<CAst::ExpressionNode>(buffer_.back());
    buffer_.pop_back();

    buffer_.push_back(std::make_shared<T>(processed_left, processed_right));
}

void SemanticCAstRewriter::visit(CAst::AndNode& node) {visit_binexp<CAst::AndNode>(node);}
void SemanticCAstRewriter::visit(CAst::OrNode& node) {visit_binexp<CAst::OrNode>(node);}
void SemanticCAstRewriter::visit(CAst::BitwiseXorNode& node) {visit_binexp<CAst::BitwiseXorNode>(node);}
void SemanticCAstRewriter::visit(CAst::BitwiseAndNode& node) {visit_binexp<CAst::BitwiseAndNode>(node);}
void SemanticCAstRewriter::visit(CAst::BitwiseOrNode& node) {visit_binexp<CAst::BitwiseOrNode>(node);}
void SemanticCAstRewriter::visit(CAst::BitwiseLeftShiftNode& node) {visit_binexp<CAst::BitwiseLeftShiftNode>(node);}
void SemanticCAstRewriter::visit(CAst::BitwiseRightShiftNode& node) {visit_binexp<CAst::BitwiseRightShiftNode>(node);}

void SemanticCAstRewriter::visit(CAst::EqualNode& node) {visit_binexp<CAst::EqualNode>(node);}
void SemanticCAstRewriter::visit(CAst::NotEqualNode& node) {visit_binexp<CAst::NotEqualNode>(node);}
void SemanticCAstRewriter::visit(CAst::GreaterNode& node) {visit_binexp<CAst::GreaterNode>(node);}
void SemanticCAstRewriter::visit(CAst::GreaterEqNode& node) {visit_binexp<CAst::GreaterEqNode>(node);}
void SemanticCAstRewriter::visit(CAst::LessNode& node) {visit_binexp<CAst::LessNode>(node);}
void SemanticCAstRewriter::visit(CAst::LessEqNode& node) {visit_binexp<CAst::LessEqNode>(node);}

void SemanticCAstRewriter::visit(CAst::ModNode& node) {visit_binexp<CAst::ModNode>(node);}
void SemanticCAstRewriter::visit(CAst::DivNode& node) {visit_binexp<CAst::DivNode>(node);}
void SemanticCAstRewriter::visit(CAst::MultNode& node) {visit_binexp<CAst::MultNode>(node);}
void SemanticCAstRewriter::visit(CAst::PlusNode& node) {visit_binexp<CAst::PlusNode>(node);}
void SemanticCAstRewriter::visit(CAst::MinusNode& node) {visit_binexp<CAst::MinusNode>(node);}

void SemanticCAstRewriter::visit(CAst::IntegerValueNode& node) {
    buffer_.push_back(std::make_shared<CAst::IntegerValueNode>(node.value_));
}

void SemanticCAstRewriter::visit(CAst::IfNode& node) {
    node.cond_->accept(*this);
    auto processed_cond = As<CAst::ExpressionNode>(buffer_.back());
    buffer_.pop_back();
    node.then_->accept(*this);
    auto processed_then = As<CAst::StatementNode>(buffer_.back());
    buffer_.pop_back();
    if (!node.alt_) {
        buffer_.push_back(
            std::make_shared<CAst::IfNode>(
                processed_cond,
                processed_then
        ));
        return;
    }
    node.alt_.value()->accept(*this);
    auto processed_alt = As<CAst::StatementNode>(buffer_.back());
    buffer_.pop_back();
        buffer_.push_back(
            std::make_shared<CAst::IfNode>(
                processed_cond,
                processed_then,
                processed_alt
        ));
}

void SemanticCAstRewriter::visit(CAst::TernaryNode& node) {
    node.cond_->accept(*this);
    auto processed_cond = As<CAst::ExpressionNode>(buffer_.back());
    buffer_.pop_back();
    node.then_->accept(*this);
    auto processed_then = As<CAst::ExpressionNode>(buffer_.back());
    buffer_.pop_back();
    node.alt_->accept(*this);
    auto processed_alt = As<CAst::ExpressionNode>(buffer_.back());
    buffer_.pop_back();
    buffer_.push_back(std::make_shared<CAst::TernaryNode>(
        processed_cond,
        processed_then,
        processed_alt
    ));
}

void SemanticCAstRewriter::visit(CAst::ForNode& node) {
    symbol_table_.emplace_back();

    node.init_->accept(*this);
    auto processed_init = As<CAst::BlockElementNode>(buffer_.back());
    buffer_.pop_back();

    node.body_->accept(*this);
    auto processed_body = As<CAst::StatementNode>(buffer_.back());
    buffer_.pop_back();

    std::optional<std::shared_ptr<CAst::ExpressionNode>> processed_cond = std::nullopt;
    if (node.cond_) {
        node.cond_.value()->accept(*this);
        processed_cond = As<CAst::ExpressionNode>(buffer_.back());
        buffer_.pop_back();
    }
    std::optional<std::shared_ptr<CAst::ExpressionNode>> processed_post = std::nullopt;
    if (node.post_) {
        node.post_.value()->accept(*this);
        processed_post = As<CAst::ExpressionNode>(buffer_.back());
        buffer_.pop_back();
    }

    symbol_table_.pop_back();

    buffer_.push_back(
        std::make_shared<CAst::ForNode>(
            processed_init,
            processed_cond,
            processed_post,
            processed_body,
            node.label_
        )
    );

}

void SemanticCAstRewriter::visit(CAst::DoWhileNode& node) {
    node.cond_->accept(*this);
    auto processed_cond = As<CAst::ExpressionNode>(buffer_.back());
    buffer_.pop_back();
    node.body_->accept(*this);
    auto processed_body = As<CAst::StatementNode>(buffer_.back());
    buffer_.pop_back();
    buffer_.push_back(std::make_shared<CAst::DoWhileNode>(
        processed_cond,
        processed_body,
        node.label_ 
    ));
}

void SemanticCAstRewriter::visit(CAst::WhileNode& node) {
    node.cond_->accept(*this);
    auto processed_cond = As<CAst::ExpressionNode>(buffer_.back());
    buffer_.pop_back();
    node.body_->accept(*this);
    auto processed_body = As<CAst::StatementNode>(buffer_.back());
    buffer_.pop_back();
    buffer_.push_back(std::make_shared<CAst::WhileNode>(
        processed_cond,
        processed_body,
        node.label_ 
    ));
}

void SemanticCAstRewriter::visit(CAst::BreakNode& node) {
    buffer_.push_back(std::make_shared<CAst::BreakNode>(node.label_));
}

void SemanticCAstRewriter::visit(CAst::ContinueNode& node) {
    buffer_.push_back(std::make_shared<CAst::ContinueNode>(node.label_));
}

std::optional<SymbolEntry> SemanticCAstRewriter::lookup_symbol_current_scope(std::string name) {
    if (symbol_table_.empty()) {return std::nullopt;}
    auto& m = symbol_table_.back();
    auto entry = m.find(name);
    if (entry == m.end())   return std::nullopt;
    return entry->second;
}

std::optional<SymbolEntry> SemanticCAstRewriter::lookup_symbol(std::string name) {
    if (symbol_table_.empty()) {return std::nullopt;}
    for(auto m = symbol_table_.rbegin(); m != symbol_table_.rend(); m++) {
        auto entry = m->find(name);
        if (entry == m->end()) continue;
        return entry->second;
    }
    return std::nullopt;
}
void SemanticCAstRewriter::insert_symbol(std::string key, SymbolEntry value) {
    if(symbol_table_.empty()) throw std::runtime_error("Cannot insert into a symbol table with an empty map stack.");
    auto& m = symbol_table_.back();
    m[key] = value;
}

std::shared_ptr<CAst::ProgramNode> SemanticCAstRewriter::process(CAst::ProgramNode& node) {
    throw std::runtime_error("process() not implemented in base rewriter");
}

} // namespace Semantic

