#include "src/backend/x86_64_asm/tacky_to_asm.h"

namespace Backend {

void PseudoReplacerVisitor::visit(ASM::PseudoNode& node) {
    if (stack_offsets_.count(node.name_) == 0) {
        current_offset_ -= 4;
        stack_offsets_[node.name_] = current_offset_;
    }
    buffer_.push_back(
        std::make_shared<ASM::StackNode>(stack_offsets_[node.name_])
    );
}

void PseudoReplacerVisitor::visit(ASM::FunctionNode& node) {
    std::vector<std::shared_ptr<ASM::InstructionNode>> replaced_instructions;
    current_offset_ = 0;
    stack_offsets_.clear();
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
    int stack_offset = (-1*current_offset_ + 15)/16;
    stack_offset*=16;
    buffer_.push_back(
        std::make_shared<ASM::FunctionNode>(
            node.name_,
            std::move(replaced_instructions),
            stack_offset
        )
    );
}


} // namespace Backend