#include "src/codegen/tacky_to_asm/tacky_to_asm.h"

namespace Codegen {

void InstructionFixUpVisitor::visit(ASM::FunctionNode& node) {
    int aligned_stack_size = (max_stack_offset_ + 15)/16;
    auto stolen_instructions = node.instructions_;
    stolen_instructions.insert(
        stolen_instructions.begin(),
        std::make_shared<ASM::AllocateStackNode>(aligned_stack_size)
    );
    buffer_.push_back(
        std::make_shared<ASM::FunctionNode>(
            node.name_,
            std::move(stolen_instructions)
        )
    );
}

void InstructionFixUpVisitor::visit(ASM::MovNode& node) {
    bool must_split_dual_memory_access = false;
    auto src_as_stack = std::dynamic_pointer_cast<ASM::StackNode>(node.src_);
    auto dst_as_stack = std::dynamic_pointer_cast<ASM::StackNode>(node.dst_);
    if (dst_as_stack && src_as_stack) {
        must_split_dual_memory_access = true;
    }
    if (must_split_dual_memory_access) {
        auto swap_register = std::make_shared<ASM::RegisterNode>(ASM::Register::R10);
        auto first_move = std::make_shared<ASM::MovNode>(node.src_, swap_register);
        auto second_move = std::make_shared<ASM::MovNode>(swap_register, node.dst_);
        buffer_.push_back(std::move(first_move));
        buffer_.push_back(std::move(second_move));
        return;
    }
    buffer_.push_back(std::make_shared<ASM::MovNode>(node.src_, node.dst_));
}


} // namespace Codegen
