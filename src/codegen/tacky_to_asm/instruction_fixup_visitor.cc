#include "src/codegen/tacky_to_asm/tacky_to_asm.h"

#include <iostream>

namespace Codegen {

void InstructionFixUpVisitor::visit(ASM::FunctionNode& node) {
    std::vector<std::shared_ptr<ASM::InstructionNode>> processed_instructions;
    for(auto& instruction : node.instructions_) {
        instruction->accept(*this);
        while(!buffer_.empty()) {
            auto processed_instruction = As<ASM::InstructionNode>(
                buffer_.front(),
                std::string("Failed to cast buffered instruction as ASM::InstructionNode")
            );
            buffer_.pop_front();
            processed_instructions.push_back(processed_instruction);
        }
    }
    int aligned_stack_size = (8 + max_stack_offset_ + 15)/16;
    aligned_stack_size *= 16;
    processed_instructions.insert(
        processed_instructions.begin(),
        std::make_shared<ASM::AllocateStackNode>(aligned_stack_size)
    );
    buffer_.push_back(
        std::make_shared<ASM::FunctionNode>(
            node.name_,
            std::move(processed_instructions)
        )
    );
}

void InstructionFixUpVisitor::visit(ASM::MovNode& node) {
    bool must_split_dual_memory_access = false;
    node.src_->accept(*this);
    auto fixed_src = As<ASM::OperandNode>(buffer_.back(), "Failed to cast buffered src to ASM::OperandNode.");
    buffer_.pop_back();
    auto src_as_stack = std::dynamic_pointer_cast<ASM::StackNode>(fixed_src);
    node.dst_->accept(*this);
    auto fixed_dst = As<ASM::OperandNode>(buffer_.back(), "Failed to cast buffered dst to ASM::OperandNode.");
    buffer_.pop_back();
    auto dst_as_stack = std::dynamic_pointer_cast<ASM::StackNode>(fixed_dst);
    if (dst_as_stack && src_as_stack) {
        must_split_dual_memory_access = true;
    }
    if (must_split_dual_memory_access) {
        auto swap_register = std::make_shared<ASM::RegisterNode>(ASM::Register::R10);
        auto first_move = std::make_shared<ASM::MovNode>(fixed_src, swap_register);
        auto second_move = std::make_shared<ASM::MovNode>(swap_register, fixed_dst);
        buffer_.push_back(std::move(first_move));
        buffer_.push_back(std::move(second_move));
        return;
    }
    buffer_.push_back(std::make_shared<ASM::MovNode>(node.src_, node.dst_));
}

void InstructionFixUpVisitor::visit(ASM::DivNode& node) {
    auto src_as_imm = std::dynamic_pointer_cast<ASM::ImmNode>(node.src_);
    if (src_as_imm) {
        auto new_target_location = std::make_shared<ASM::RegisterNode>(ASM::Register::R10);
        buffer_.push_back(std::make_shared<ASM::MovNode>(
            node.src_,
            new_target_location
        ));
        buffer_.push_back(std::make_shared<ASM::DivNode>(
            new_target_location
        ));
    } else {
        buffer_.push_back(std::make_shared<ASM::DivNode>(
            node.src_
        ));
    }
}

void InstructionFixUpVisitor::visit(ASM::AddNode& node) {
    auto left_as_stack_locations = std::dynamic_pointer_cast<ASM::StackNode>(node.left_);
    auto right_as_stack_locations = std::dynamic_pointer_cast<ASM::StackNode>(node.right_);
    if (
        left_as_stack_locations &&
        right_as_stack_locations
    ) {
        auto r10 = std::make_shared<ASM::RegisterNode>(ASM::Register::R10);
        buffer_.push_back(std::make_shared<ASM::MovNode>(
            node.left_,
            r10
        ));
        buffer_.push_back(std::make_shared<ASM::AddNode>(
            r10,
            node.right_
        ));
    } else {
        buffer_.push_back(std::make_shared<ASM::AddNode>(
            node.left_,
            node.right_
        ));
    }
}

void InstructionFixUpVisitor::visit(ASM::SubNode& node) {
    auto left_as_stack_locations = std::dynamic_pointer_cast<ASM::StackNode>(node.left_);
    auto right_as_stack_locations = std::dynamic_pointer_cast<ASM::StackNode>(node.right_);
    if (
        left_as_stack_locations &&
        right_as_stack_locations
    ) {
        auto r10 = std::make_shared<ASM::RegisterNode>(ASM::Register::R10);
        buffer_.push_back(std::make_shared<ASM::MovNode>(
            node.left_,
            r10
        ));
        buffer_.push_back(std::make_shared<ASM::SubNode>(
            r10,
            node.right_
        ));
    } else {
        buffer_.push_back(std::make_shared<ASM::SubNode>(
            node.left_,
            node.right_
        ));
    }
}

void InstructionFixUpVisitor::visit(ASM::MultNode& node) {
    auto right_operand_as_stack = std::dynamic_pointer_cast<ASM::StackNode>(node.right_);
    if (right_operand_as_stack) {
        auto r11 = std::make_shared<ASM::RegisterNode>(ASM::Register::R11);
        buffer_.push_back(std::make_shared<ASM::MovNode>(
            node.right_,
            r11
        ));
        buffer_.push_back(std::make_shared<ASM::MultNode>(
            node.left_,
            r11
        ));
        buffer_.push_back(std::make_shared<ASM::MovNode>(
            r11,
            node.right_
        ));
    } else {
        buffer_.push_back(std::make_shared<ASM::MultNode>(
            node.left_,
            node.right_
        ));
    }
}



} // namespace Codegen
