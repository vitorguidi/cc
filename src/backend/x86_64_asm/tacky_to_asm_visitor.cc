#include "src/backend/x86_64_asm/tacky_to_asm.h"
#include <algorithm>

namespace Backend {

std::vector<ASM::Register> registers = {
    ASM::Register::DI,
    ASM::Register::SI,
    ASM::Register::DX,
    ASM::Register::CX,
    ASM::Register::R8,
    ASM::Register::R9,
};

template<std::derived_from<ASM::BinInstructionNode> T>
void TackyToAsmVisitor::visit_binexp(Tacky::BinaryOpNode& node) {
    node.left_->accept(*this);
    auto left_operand = As<ASM::OperandNode>(
        result_buffer_.back(), "Failed to cast left operand to ASM::OperandNode");
    result_buffer_.pop_back();

    node.right_->accept(*this);
    auto right_operand = As<ASM::OperandNode>(
        result_buffer_.back(), "Failed to cast right operand to ASM::OperandNode");
    result_buffer_.pop_back();

    node.dst_->accept(*this);
    auto dst_operand = As<ASM::OperandNode>(
        result_buffer_.back(), "Failed to cast dst operand to ASM::OperandNode");
    result_buffer_.pop_back();

    instruction_buffer_.push_back(std::make_shared<ASM::MovNode>(
        left_operand,
        dst_operand
    ));

    instruction_buffer_.push_back(std::make_shared<T>(
        right_operand,
        dst_operand
    ));

    result_buffer_.push_back(std::make_shared<ASM::NullNode>());
}

template<std::derived_from<ASM::UnaryInstructionNode> T>
void TackyToAsmVisitor::visit_unexp(Tacky::UnaryNode& node) {
    node.src_->accept(*this);

    auto converted_src = As<ASM::OperandNode>(
        result_buffer_.back(), "Failed to cast src operand to ASM::OperandNode");
    result_buffer_.pop_back();

    node.dst_->accept(*this);

    auto converted_dst = As<ASM::OperandNode>(
        result_buffer_.back(), "Failed to cast dst operand to ASM::OperandNode");
    result_buffer_.pop_back();

    instruction_buffer_.push_back(std::make_shared<ASM::MovNode>(converted_src, converted_dst));
    instruction_buffer_.push_back(std::make_shared<T>(converted_dst));
    result_buffer_.push_back(std::make_shared<ASM::NullNode>());
}

void TackyToAsmVisitor::visit_relational_exp(ASM::ConditionCode cc, Tacky::RelationalOpNode& node) {
    node.left_->accept(*this);

    auto converted_left = As<ASM::OperandNode>(
        result_buffer_.back(), "Failed to cast left operand to ASM::OperandNode");
    result_buffer_.pop_back();

    node.right_->accept(*this);

    auto converted_right = As<ASM::OperandNode>(
        result_buffer_.back(), "Failed to cast left operand to ASM::OperandNode");
    result_buffer_.pop_back();

    node.dst_->accept(*this);

    auto converted_dst = As<ASM::OperandNode>(
        result_buffer_.back(), "Failed to cast dst operand to ASM::OperandNode");
    result_buffer_.pop_back();

    instruction_buffer_.push_back(std::make_shared<ASM::CmpNode>(converted_right, converted_left));
    instruction_buffer_.push_back(std::make_shared<ASM::MovNode>(
        std::make_shared<ASM::ImmNode>(0),
        converted_dst
    ));
    instruction_buffer_.push_back(std::make_shared<ASM::SetCCNode>(cc, converted_dst));
    result_buffer_.push_back(std::make_shared<ASM::NullNode>());
}

void TackyToAsmVisitor::visit(Tacky::FunctionCallNode& node) {
    size_t register_args = std::min(node.args_.size(), size_t{6});
    size_t stack_args = node.args_.size() - register_args;
    size_t total_args = register_args + stack_args;
    if (register_args + stack_args != node.args_.size()) {
        throw std::runtime_error("Bad arg count: expected " + std::to_string(node.args_.size()) + ", got " + std::to_string(total_args));
    }
    int padding = 0;
    if (stack_args%2) {
        padding = 8;
    }

    if (padding) {
        instruction_buffer_.push_back(std::make_shared<ASM::AllocateStackNode>(padding));
    }

    for(size_t i=0;i<register_args;i++) {
        auto reg = std::make_shared<ASM::RegisterNode>(registers[i]);
        node.args_[i]->accept(*this);
        auto converted_arg = As<ASM::OperandNode>(
        result_buffer_.back(), "Failed to cast function call arg to ASM::OperandNode");
        result_buffer_.pop_back();
        instruction_buffer_.push_back(std::make_shared<ASM::MovNode>(
            converted_arg,
            reg
        ));
    }

    for(size_t i=node.args_.size()-1;register_args>0 && i>=register_args;i--) {
        node.args_[i]->accept(*this);
        auto converted_arg = As<ASM::OperandNode>(
        result_buffer_.back(), "Failed to cast function call arg to ASM::OperandNode");
        result_buffer_.pop_back();
        if( std::dynamic_pointer_cast<ASM::ImmNode>(converted_arg) != nullptr ||
            std::dynamic_pointer_cast<ASM::RegisterNode>(converted_arg)) {

            instruction_buffer_.push_back(std::make_shared<ASM::PushNode>(converted_arg));
        } else {
            auto ax = std::make_shared<ASM::RegisterNode>(ASM::Register::AX);
            instruction_buffer_.push_back(
                std::make_shared<ASM::MovNode>(
                    converted_arg,
                    ax
                )
            );
            instruction_buffer_.push_back(std::make_shared<ASM::PushNode>(ax));
        }
    }

    instruction_buffer_.push_back(std::make_shared<ASM::CallNode>(node.name_));
    auto bytes_to_remove = 8*stack_args + padding;
    if (bytes_to_remove) {
        instruction_buffer_.push_back(std::make_shared<ASM::DeallocateStackNode>(bytes_to_remove));
    }
    node.dst_->accept(*this);
    auto converted_dst = As<ASM::OperandNode>(
        result_buffer_.back(), "Failed to cast function call dst to ASM::OperandNode");
    result_buffer_.pop_back();

    instruction_buffer_.push_back(std::make_shared<ASM::MovNode>(
        std::make_shared<ASM::RegisterNode>(ASM::Register::AX),
        converted_dst
    ));
    result_buffer_.push_back(std::make_shared<ASM::NullNode>());

}

void TackyToAsmVisitor::visit_conditional_jump(ASM::ConditionCode cc, Tacky::ConditionalJumpNode& node) {
    auto dst = std::make_shared<ASM::LabelNode>(node.dst_->name_);

    node.operand_->accept(*this);
    auto operand = As<ASM::OperandNode>(
        result_buffer_.back(), "Failed to cast operand in JumpIfZero to ASM::OperandNode");
    result_buffer_.pop_back();

    instruction_buffer_.push_back(std::make_shared<ASM::CmpNode>(
        std::make_shared<ASM::ImmNode>(0),
        operand
    ));

    instruction_buffer_.push_back(std::make_shared<ASM::JumpCCNode>(cc, dst));
    result_buffer_.push_back(std::make_shared<ASM::NullNode>());
}

std::shared_ptr<ASM::ProgramNode> TackyToAsmVisitor::get_asm_from_tacky(std::shared_ptr<Tacky::ProgramNode> tacky_program) {
    if (!result_buffer_.empty()) {
        throw std::runtime_error("Expected buffer to be empty before generating asm from tacky.");
    }
    tacky_program->accept(*this);
    auto asm_program = As<ASM::ProgramNode>(
        result_buffer_.back(), "Expected buffer to be empty after generating asm from tacky.");
    result_buffer_.pop_back();
    return asm_program;
}

void TackyToAsmVisitor::visit(Tacky::ProgramNode& node) {
    std::vector<std::shared_ptr<ASM::FunctionNode>> converted_functions;
    for(auto& function : node.functions_) {
        function->accept(*this);
        auto converted_fn = As<ASM::FunctionNode>(
            result_buffer_.back(), std::string("Failed to cast buffered function to ASM::FunctionNode"));
        result_buffer_.pop_back();
        converted_functions.push_back(std::move(converted_fn));
    }

    std::reverse(converted_functions.begin(), converted_functions.end());
    std::shared_ptr<ASM::ProgramNode> converted_program = 
        std::make_shared<ASM::ProgramNode>(std::move(converted_functions));
    result_buffer_.push_back(std::move(converted_program));
}

void TackyToAsmVisitor::visit(Tacky::FunctionNode& node) {

    size_t register_args = std::min(node.args_.size(), size_t{6});

    // previously, rbp and the ret address were pushed, so 16bytes already taken

    for(size_t i=0;i<std::min(node.args_.size(), size_t{6});i++) {
        auto arg_as_operand = std::make_shared<ASM::PseudoNode>(node.args_[i]);
        instruction_buffer_.push_back(std::make_shared<ASM::MovNode>(
            std::make_shared<ASM::RegisterNode>(registers[i]),
            arg_as_operand
        ));
    }

    int arg_stack_position = 16;

    for(size_t i=register_args;i<node.args_.size();i++) {
        auto arg_as_operand = std::make_shared<ASM::PseudoNode>(node.args_[i]);
        instruction_buffer_.push_back(std::make_shared<ASM::MovNode>(
            std::make_shared<ASM::StackNode>(arg_stack_position),
            arg_as_operand
        ));
        arg_stack_position+=8;
    }

    for(auto& tacky_instruction : node.instructions_) {
        tacky_instruction->accept(*this);
        result_buffer_.pop_back();
    }

    std::vector<std::shared_ptr<ASM::InstructionNode>> converted_instructions;

    while(!instruction_buffer_.empty()) {
        // why does a fibonacci function node show up here?
        auto converted_instruction = As<ASM::InstructionNode>(
            instruction_buffer_.back(), "Unable to cast buffered instruction to ASM::InstructionNode");
        instruction_buffer_.pop_back();
        converted_instructions.push_back(std::move(converted_instruction));
    }

    std::reverse(converted_instructions.begin(), converted_instructions.end());
    auto converted_fn = std::make_shared<ASM::FunctionNode>(node.name_, converted_instructions, -1);
    result_buffer_.push_back(std::move(converted_fn));
}

void TackyToAsmVisitor::visit(Tacky::ReturnNode& node) {
    node.value_->accept(*this);
    auto casted_value = As<ASM::OperandNode>(
        result_buffer_.back(), "Failed to cast buffered value to ASM::OperandNode");
    result_buffer_.pop_back();
    auto return_destination = std::make_shared<ASM::RegisterNode>(ASM::Register::AX);
    auto mov_instruction = std::make_shared<ASM::MovNode>(casted_value, return_destination);
    auto ret_instruction = std::make_shared<ASM::RetNode>();
    instruction_buffer_.push_back(std::move(mov_instruction));
    instruction_buffer_.push_back(std::move(ret_instruction));
    result_buffer_.push_back(std::make_shared<ASM::NullNode>());
}

void TackyToAsmVisitor::visit(Tacky::NullNode& node) {
    result_buffer_.push_back(std::make_shared<ASM::NullNode>());
}

// unary exps
void TackyToAsmVisitor::visit(Tacky::BitwiseNotNode& node) {visit_unexp<ASM::BitwiseNotNode>(node);}
void TackyToAsmVisitor::visit(Tacky::ComplementNode& node) {visit_unexp<ASM::ComplementNode>(node);}

void TackyToAsmVisitor::visit(Tacky::NotNode& node) {
    node.src_->accept(*this);

    auto converted_src = As<ASM::OperandNode>(
        result_buffer_.back(), "Failed to cast src operand to ASM::OperandNode");
    result_buffer_.pop_back();

    node.dst_->accept(*this);

    auto converted_dst = As<ASM::OperandNode>(
        result_buffer_.back(), "Failed to cast dst operand to ASM::OperandNode");
    result_buffer_.pop_back();

    instruction_buffer_.push_back(std::make_shared<ASM::CmpNode>(
        std::make_shared<ASM::ImmNode>(0),
        converted_src
    ));
    instruction_buffer_.push_back(std::make_shared<ASM::MovNode>(
        std::make_shared<ASM::ImmNode>(0),
        converted_dst
    ));
    instruction_buffer_.push_back(std::make_shared<ASM::SetCCNode>(
        ASM::ConditionCode::EQUAL,
        converted_dst
    ));
    result_buffer_.push_back(std::make_shared<ASM::NullNode>());
}

// binary arithmetic exps
void TackyToAsmVisitor::visit(Tacky::MultNode& node) {visit_binexp<ASM::MultNode>(node);}
void TackyToAsmVisitor::visit(Tacky::PlusNode& node) {visit_binexp<ASM::AddNode>(node);}
void TackyToAsmVisitor::visit(Tacky::MinusNode& node) {visit_binexp<ASM::SubNode>(node);}

// binary bitwise exps
void TackyToAsmVisitor::visit(Tacky::BitwiseAndNode& node) {visit_binexp<ASM::BitwiseAndNode>(node);}
void TackyToAsmVisitor::visit(Tacky::BitwiseOrNode& node) {visit_binexp<ASM::BitwiseOrNode>(node);}
void TackyToAsmVisitor::visit(Tacky::BitwiseXorNode& node) {visit_binexp<ASM::BitwiseXorNode>(node);}
void TackyToAsmVisitor::visit(Tacky::BitwiseLeftShiftNode& node) {visit_binexp<ASM::SalNode>(node);}
void TackyToAsmVisitor::visit(Tacky::BitwiseRightShiftNode& node) {visit_binexp<ASM::SarNode>(node);}

// division is a bit of a snowflake
void TackyToAsmVisitor::visit(Tacky::DivNode& node) {
    node.left_->accept(*this);
    auto left_operand = As<ASM::OperandNode>(
        result_buffer_.back(), "Failed to cast left operand in div to ASM::OperandNode");
    result_buffer_.pop_back();

    node.right_->accept(*this);
    auto right_operand = As<ASM::OperandNode>(
        result_buffer_.back(), "Failed to cast right operand in div to ASM::OperandNode");
    result_buffer_.pop_back();

    node.dst_->accept(*this);
    auto dst_operand = As<ASM::OperandNode>(
        result_buffer_.back(), "Failed to cast dst operand in div to ASM::OperandNode");
    result_buffer_.pop_back();

    instruction_buffer_.push_back(std::make_shared<ASM::MovNode>(
        left_operand,
        std::make_shared<ASM::RegisterNode>(ASM::Register::AX)
    ));

    instruction_buffer_.push_back(std::make_shared<ASM::CDQNode>());

    instruction_buffer_.push_back(std::make_shared<ASM::DivNode>(right_operand));
    instruction_buffer_.push_back(std::make_shared<ASM::MovNode>(
        std::make_shared<ASM::RegisterNode>(ASM::Register::AX),
        dst_operand
    ));
    result_buffer_.push_back(std::make_shared<ASM::NullNode>());

}

void TackyToAsmVisitor::visit(Tacky::ModNode& node) {
    node.left_->accept(*this);
    auto left_operand = As<ASM::OperandNode>(
        result_buffer_.back(), "Failed to cast left operand in div to ASM::OperandNode");
    result_buffer_.pop_back();

    node.right_->accept(*this);
    auto right_operand = As<ASM::OperandNode>(
        result_buffer_.back(), "Failed to cast right operand in div to ASM::OperandNode");
    result_buffer_.pop_back();

    node.dst_->accept(*this);
    auto dst_operand = As<ASM::OperandNode>(
        result_buffer_.back(), "Failed to cast dst operand in div to ASM::OperandNode");
    result_buffer_.pop_back();

    instruction_buffer_.push_back(std::make_shared<ASM::MovNode>(
        left_operand,
        std::make_shared<ASM::RegisterNode>(ASM::Register::AX)
    ));

    instruction_buffer_.push_back(std::make_shared<ASM::CDQNode>());

    instruction_buffer_.push_back(std::make_shared<ASM::DivNode>(right_operand));
    instruction_buffer_.push_back(std::make_shared<ASM::MovNode>(
        std::make_shared<ASM::RegisterNode>(ASM::Register::DX),
        dst_operand
    ));
    result_buffer_.push_back(std::make_shared<ASM::NullNode>());
}


// relational binexps
void TackyToAsmVisitor::visit(Tacky::EqualNode& node) {visit_relational_exp(ASM::ConditionCode::EQUAL, node);}
void TackyToAsmVisitor::visit(Tacky::NotEqualNode& node) {visit_relational_exp(ASM::ConditionCode::NOT_EQUAL, node);}
void TackyToAsmVisitor::visit(Tacky::GreaterNode& node) {visit_relational_exp(ASM::ConditionCode::GREATER, node);}
void TackyToAsmVisitor::visit(Tacky::GreaterEqNode& node) {visit_relational_exp(ASM::ConditionCode::GREATER_EQ, node);}
void TackyToAsmVisitor::visit(Tacky::LessNode& node) {visit_relational_exp(ASM::ConditionCode::LESS, node);}
void TackyToAsmVisitor::visit(Tacky::LessEqNode& node) {visit_relational_exp(ASM::ConditionCode::LESS_EQ, node);}


// jump nodes
void TackyToAsmVisitor::visit(Tacky::JumpNode& node) {
    auto dst = std::make_shared<ASM::LabelNode>(node.dst_->name_);
    instruction_buffer_.push_back(std::make_shared<ASM::JumpNode>(dst));
    result_buffer_.push_back(std::make_shared<ASM::NullNode>());
}

void TackyToAsmVisitor::visit(Tacky::JumpIfZeroNode& node) {visit_conditional_jump(ASM::ConditionCode::EQUAL, node);}
void TackyToAsmVisitor::visit(Tacky::JumpIfNotZeroNode& node) {visit_conditional_jump(ASM::ConditionCode::NOT_EQUAL, node);}

void TackyToAsmVisitor::visit(Tacky::LabelNode& node) {
    instruction_buffer_.push_back(std::make_shared<ASM::LabelNode>(node.name_));
    result_buffer_.push_back(std::make_shared<ASM::NullNode>());
}

void TackyToAsmVisitor::visit(Tacky::MovNode& node) {
    node.src_->accept(*this);
    auto src = As<ASM::OperandNode>(
        result_buffer_.back(), "Failed to cast src to ASM::OperandNode");
    result_buffer_.pop_back();

    node.dst_->accept(*this);
    auto dst = As<ASM::OperandNode>(
        result_buffer_.back(), "Failed to cast src to ASM::OperandNode");
    result_buffer_.pop_back();
    instruction_buffer_.push_back(std::make_shared<ASM::MovNode>(src,dst));
    result_buffer_.push_back(std::make_shared<ASM::NullNode>());
}

void TackyToAsmVisitor::visit(Tacky::IntegerNode& node) {
    result_buffer_.push_back(std::make_shared<ASM::ImmNode>(node.value_));
}

void TackyToAsmVisitor::visit(Tacky::VariableNode& node) {
    result_buffer_.push_back(std::make_shared<ASM::PseudoNode>(node.name_));
}

} //namespace Backend