#include "src/codegen/tacky_to_asm/tacky_to_asm.h"

namespace Codegen {

void ASMRewriteVisitor::visit(ASM::ProgramNode& node) {
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

void ASMRewriteVisitor::visit(ASM::FunctionNode& node) {
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
            std::move(replaced_instructions)
        )
    );
}

void ASMRewriteVisitor::visit(ASM::NegNode& node) {
    node.src_->accept(*this);
    std::shared_ptr<ASM::OperandNode> casted_src = As<ASM::OperandNode>(
        buffer_.back(),
        "Failed to cast buffered operand into ASM::OperandNode"
    );
    buffer_.pop_back();
    buffer_.push_back(
        std::make_shared<ASM::NegNode>(
            casted_src
        )
    );
}

void ASMRewriteVisitor::visit(ASM::NotNode& node) {
    node.src_->accept(*this);
    std::shared_ptr<ASM::OperandNode> casted_src = As<ASM::OperandNode>(
        buffer_.back(),
        "Failed to cast buffered operand into ASM::OperandNode"
    );
    buffer_.pop_back();
    buffer_.push_back(
        std::make_shared<ASM::NotNode>(
            casted_src
        )
    );
}

void ASMRewriteVisitor::visit(ASM::MovNode& node) {
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

void ASMRewriteVisitor::visit(ASM::RetNode& node) {
    buffer_.push_back(std::make_shared<ASM::RetNode>());
}

void ASMRewriteVisitor::visit(ASM::AllocateStackNode& node) {
    buffer_.push_back(std::make_shared<ASM::AllocateStackNode>(node.size_));
}

void ASMRewriteVisitor::visit(ASM::ImmNode& node) {
    buffer_.push_back(std::make_shared<ASM::ImmNode>(node.val_));
}

void ASMRewriteVisitor::visit(ASM::StackNode& node) {
    buffer_.push_back(std::make_shared<ASM::StackNode>(node.size_));
}

void ASMRewriteVisitor::visit(ASM::RegisterNode& node) {
    buffer_.push_back(std::make_shared<ASM::RegisterNode>(node.reg_));
}

void ASMRewriteVisitor::visit(ASM::PseudoNode& node) {
    buffer_.push_back(std::make_shared<ASM::PseudoNode>(node.name_));
}

std::shared_ptr<ASM::ProgramNode> ASMRewriteVisitor::get_rewritten_asm_program(std::shared_ptr<ASM::ProgramNode> program) {
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

} // namespace Codegen