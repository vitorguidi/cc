#include "src/backend/x86_64_asm/tacky_to_asm.h"

namespace Backend {

template <std::derived_from<ASM::BinInstructionNode> T>
void x86_64_ASM_RewriteVisitor::visit_binexp(ASM::BinInstructionNode& node) {
    node.left_->accept(*this);
    std::shared_ptr<ASM::OperandNode> casted_left = As<ASM::OperandNode>(
        buffer_.back(),
        "Failed to cast buffered operand into ASM::OperandNode"
    );
    buffer_.pop_back();
    node.right_->accept(*this);
    std::shared_ptr<ASM::OperandNode> casted_right = As<ASM::OperandNode>(
        buffer_.back(),
        "Failed to cast buffered operand into ASM::OperandNode"
    );
    buffer_.pop_back();
    buffer_.push_back(std::make_shared<T>(
        casted_left,
        casted_right
    ));
}

template <std::derived_from<ASM::UnaryInstructionNode> T>
void x86_64_ASM_RewriteVisitor::visit_unexp(ASM::UnaryInstructionNode& node) {
    node.src_->accept(*this);
    std::shared_ptr<ASM::OperandNode> casted_src = As<ASM::OperandNode>(
        buffer_.back(),
        "Failed to cast buffered operand into ASM::OperandNode"
    );
    buffer_.pop_back();
    buffer_.push_back(
        std::make_shared<T>(
            casted_src
        )
    );
}

void x86_64_ASM_RewriteVisitor::visit(ASM::ProgramNode& node) {
    std::vector<std::shared_ptr<ASM::FunctionNode>> replaced_functions;
    for(auto& fn : node.functions_) {
        fn->accept(*this);
        auto casted_fn = As<ASM::FunctionNode>(
            buffer_.back(), std::string("Failed to cast buffered node into ASM::FunctionNode."));
        buffer_.pop_back();
        replaced_functions.push_back(std::move(casted_fn));
    }
    buffer_.push_back(std::make_shared<ASM::ProgramNode>(replaced_functions));
}

void x86_64_ASM_RewriteVisitor::visit(ASM::FunctionNode& node) {
    std::vector<std::shared_ptr<ASM::InstructionNode>> replaced_instructions;
    for(auto& instruction : node.instructions_) {
        instruction->accept(*this);
        while(!buffer_.empty()) {
            auto casted_instruction = As<ASM::InstructionNode>(
                buffer_.front(),
                std::string("Failed to cast buffered node into ASM::InstructionNode.")
            );
            buffer_.pop_front();
            replaced_instructions.push_back(std::move(casted_instruction));
        }
    }
    buffer_.push_back(
        std::make_shared<ASM::FunctionNode>(
            node.name_,
            std::move(replaced_instructions),
            node.stack_offset_
        )
    );
}

void x86_64_ASM_RewriteVisitor::visit(ASM::ComplementNode& node) {visit_unexp<ASM::ComplementNode>(node);}
void x86_64_ASM_RewriteVisitor::visit(ASM::BitwiseNotNode& node) {visit_unexp<ASM::BitwiseNotNode>(node);}


void x86_64_ASM_RewriteVisitor::visit(ASM::MovNode& node) {
    node.src_->accept(*this);
    std::shared_ptr<ASM::OperandNode> casted_src = As<ASM::OperandNode>(
        buffer_.back(),
        "Failed to cast buffered operand into ASM::OperandNode"
    );
    buffer_.pop_back();
    node.dst_->accept(*this);
    std::shared_ptr<ASM::OperandNode> casted_dst = As<ASM::OperandNode>(
        buffer_.back(),
        "Failed to cast buffered operand into ASM::OperandNode"
    );
    buffer_.pop_back();
    buffer_.push_back(
        std::make_shared<ASM::MovNode>(
            casted_src,
            casted_dst
        )
    );
}

void x86_64_ASM_RewriteVisitor::visit(ASM::MovBNode& node) {
    node.src_->accept(*this);
    std::shared_ptr<ASM::OperandNode> casted_src = As<ASM::OperandNode>(
        buffer_.back(),
        "Failed to cast buffered operand into ASM::OperandNode"
    );
    buffer_.pop_back();
    node.dst_->accept(*this);
    std::shared_ptr<ASM::OperandNode> casted_dst = As<ASM::OperandNode>(
        buffer_.back(),
        "Failed to cast buffered operand into ASM::OperandNode"
    );
    buffer_.pop_back();
    buffer_.push_back(
        std::make_shared<ASM::MovBNode>(
            casted_src,
            casted_dst
        )
    );
}

void x86_64_ASM_RewriteVisitor::visit(ASM::DivNode& node) {
    node.src_->accept(*this);
    std::shared_ptr<ASM::OperandNode> casted_src = As<ASM::OperandNode>(
        buffer_.back(),
        "Failed to cast buffered operand into ASM::OperandNode"
    );
    buffer_.pop_back();
    buffer_.push_back(std::make_shared<ASM::DivNode>(
        casted_src
    ));
}

void x86_64_ASM_RewriteVisitor::visit(ASM::NullNode& node) {
    buffer_.push_back(std::make_shared<ASM::NullNode>());
}

// binary arithmetic exps
void x86_64_ASM_RewriteVisitor::visit(ASM::MultNode& node) {visit_binexp<ASM::MultNode>(node);}
void x86_64_ASM_RewriteVisitor::visit(ASM::AddNode& node) {visit_binexp<ASM::AddNode>(node);}
void x86_64_ASM_RewriteVisitor::visit(ASM::SubNode& node) {visit_binexp<ASM::SubNode>(node);}

// binary boolean exps
void x86_64_ASM_RewriteVisitor::visit(ASM::BitwiseAndNode& node) {visit_binexp<ASM::BitwiseAndNode>(node);}
void x86_64_ASM_RewriteVisitor::visit(ASM::BitwiseOrNode& node) {visit_binexp<ASM::BitwiseOrNode>(node);}
void x86_64_ASM_RewriteVisitor::visit(ASM::BitwiseXorNode& node) {visit_binexp<ASM::BitwiseXorNode>(node);}
void x86_64_ASM_RewriteVisitor::visit(ASM::SalNode& node) {visit_binexp<ASM::SalNode>(node);}
void x86_64_ASM_RewriteVisitor::visit(ASM::SarNode& node) {visit_binexp<ASM::SarNode>(node);}

void x86_64_ASM_RewriteVisitor::visit(ASM::CmpNode& node) {
    node.operand1_->accept(*this);
    std::shared_ptr<ASM::OperandNode> casted_operand1 = As<ASM::OperandNode>(
        buffer_.back(),
        "Failed to cast buffered operand into ASM::OperandNode"
    );
    buffer_.pop_back();

    node.operand2_->accept(*this);
    std::shared_ptr<ASM::OperandNode> casted_operand2 = As<ASM::OperandNode>(
        buffer_.back(),
        "Failed to cast buffered operand into ASM::OperandNode"
    );
    buffer_.pop_back();
    buffer_.push_back(std::make_shared<ASM::CmpNode>(casted_operand1, casted_operand2));
}

void x86_64_ASM_RewriteVisitor::visit(ASM::SetCCNode& node) {
    node.operand_->accept(*this);
    std::shared_ptr<ASM::OperandNode> casted_operand = As<ASM::OperandNode>(
        buffer_.back(),
        "Failed to cast buffered operand into ASM::OperandNode"
    );
    buffer_.pop_back();
    buffer_.push_back(std::make_shared<ASM::SetCCNode>(node.cc_, casted_operand));
}

void x86_64_ASM_RewriteVisitor::visit(ASM::JumpCCNode& node) {
    node.target_->accept(*this);
    std::shared_ptr<ASM::LabelNode> casted_target = As<ASM::LabelNode>(
        buffer_.back(),
        "Failed to cast buffered operand into ASM::LabelNode"
    );
    buffer_.pop_back();
    buffer_.push_back(std::make_shared<ASM::JumpCCNode>(
        node.cc_,
        casted_target
    ));
}

void x86_64_ASM_RewriteVisitor::visit(ASM::JumpNode& node) {
    node.target_->accept(*this);
    std::shared_ptr<ASM::LabelNode> casted_target = As<ASM::LabelNode>(
        buffer_.back(),
        "Failed to cast buffered operand into ASM::LabelNode"
    );
    buffer_.pop_back();
    buffer_.push_back(std::make_shared<ASM::JumpNode>(
        casted_target
    ));
}

void x86_64_ASM_RewriteVisitor::visit(ASM::LabelNode& node) {
    buffer_.push_back(std::make_shared<ASM::LabelNode>(node.name_));
}

void x86_64_ASM_RewriteVisitor::visit(ASM::CDQNode& node) {
    buffer_.push_back(std::make_shared<ASM::CDQNode>());
}

void x86_64_ASM_RewriteVisitor::visit(ASM::RetNode& node) {
    buffer_.push_back(std::make_shared<ASM::RetNode>());
}

void x86_64_ASM_RewriteVisitor::visit(ASM::AllocateStackNode& node) {
    buffer_.push_back(std::make_shared<ASM::AllocateStackNode>(node.size_));
}

void x86_64_ASM_RewriteVisitor::visit(ASM::ImmNode& node) {
    buffer_.push_back(std::make_shared<ASM::ImmNode>(node.val_));
}

void x86_64_ASM_RewriteVisitor::visit(ASM::StackNode& node) {
    buffer_.push_back(std::make_shared<ASM::StackNode>(node.size_));
}

void x86_64_ASM_RewriteVisitor::visit(ASM::RegisterNode& node) {
    buffer_.push_back(std::make_shared<ASM::RegisterNode>(node.reg_));
}

void x86_64_ASM_RewriteVisitor::visit(ASM::PseudoNode& node) {
    buffer_.push_back(std::make_shared<ASM::PseudoNode>(node.name_));
}

void x86_64_ASM_RewriteVisitor::visit(ASM::PushNode& node) {
    node.operand_->accept(*this);
    std::shared_ptr<ASM::OperandNode> casted_operand = As<ASM::OperandNode>(
        buffer_.back(),
        "Failed to cast buffered operand into ASM::OperandNode"
    );
    buffer_.pop_back();
    buffer_.push_back(std::make_shared<ASM::PushNode>(casted_operand));
}

void x86_64_ASM_RewriteVisitor::visit(ASM::CallNode& node) {
    buffer_.push_back(std::make_shared<ASM::CallNode>(node.name_));
}

void x86_64_ASM_RewriteVisitor::visit(ASM::DeallocateStackNode& node) {
    buffer_.push_back(std::make_shared<ASM::DeallocateStackNode>(node.size_));
}

std::shared_ptr<ASM::ProgramNode> x86_64_ASM_RewriteVisitor::get_rewritten_asm_program(std::shared_ptr<ASM::ProgramNode> program) {
    if (!buffer_.empty()) {
        throw std::runtime_error("Buffer must be empty before attempting to rewrite ASM Ast");
    }
    program->accept(*this);
    if (buffer_.size() != 1) {
        throw std::runtime_error("Buffer must have only the program node post AST rewrite");
    }
    auto result = buffer_.back();
    buffer_.pop_back();
    return As<ASM::ProgramNode>(result, "Failed to cast result into ASM::ProgramNode.");
}

} // namespace Backend