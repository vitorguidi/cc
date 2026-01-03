#include "src/codegen/tacky_to_asm/tacky_to_asm.h"
#include <algorithm>

namespace Codegen {

std::shared_ptr<ASM::ProgramNode> TackyToAsmVisitor::get_asm_from_tacky(std::shared_ptr<Tacky::ProgramNode> tacky_program) {
    if (!buffer_.empty()) {
        throw std::runtime_error("Expected buffer to be empty before generating asm from tacky.");
    }
    tacky_program->accept(*this);
    auto asm_program = As<ASM::ProgramNode>(
        buffer_.back(), "Expected buffer to be empty after generating asm from tacky.");
    buffer_.pop_back();
    return asm_program;
}

void TackyToAsmVisitor::visit(Tacky::ProgramNode& node) {
    for(auto& function : node.functions_) {
        function->accept(*this);
    }
    std::vector<std::shared_ptr<ASM::FunctionNode>> converted_functions;
    while(!buffer_.empty()) {
        auto converted_fn = As<ASM::FunctionNode>(
            buffer_.back(), std::string("Failed to cast buffered function to ASM::FunctionNode"));
        buffer_.pop_back();
        converted_functions.push_back(std::move(converted_fn));
    }
    std::reverse(converted_functions.begin(), converted_functions.end());
    std::shared_ptr<ASM::ProgramNode> converted_program = 
        std::make_shared<ASM::ProgramNode>(std::move(converted_functions));
    buffer_.push_back(std::move(converted_program));
}

void TackyToAsmVisitor::visit(Tacky::FunctionNode& node) {
    for(auto& tacky_instruction : node.instructions_) {
        tacky_instruction->accept(*this);
    }
    std::vector<std::shared_ptr<ASM::InstructionNode>> converted_instructions;
    while(!buffer_.empty()) {
        auto converted_instruction = As<ASM::InstructionNode>(
            buffer_.back(), "Unable to cast buffered instruction to ASM::InstructionNode");
        buffer_.pop_back();
        converted_instructions.push_back(std::move(converted_instruction));
    }
    std::reverse(converted_instructions.begin(), converted_instructions.end());
    auto converted_fn = std::make_shared<ASM::FunctionNode>(node.name_, converted_instructions);
    buffer_.push_back(std::move(converted_fn));
}

void TackyToAsmVisitor::visit(Tacky::ReturnNode& node) {
    node.value_->accept(*this);
    auto casted_value = As<ASM::OperandNode>(
        buffer_.back(), "Failed to cast buffered value to ASM::OperandNode");
    buffer_.pop_back();
    auto return_destination = std::make_shared<ASM::RegisterNode>(ASM::Register::AX);
    auto mov_instruction = std::make_shared<ASM::MovNode>(casted_value, return_destination);
    auto ret_instruction = std::make_shared<ASM::RetNode>();
    buffer_.push_back(std::move(mov_instruction));
    buffer_.push_back(std::move(ret_instruction));
}

void TackyToAsmVisitor::visit(Tacky::ComplementNode& node) {
    // todo: move this boilerplate to parent unary class
    node.src_->accept(*this);
    auto converted_src = As<ASM::OperandNode>(
        buffer_.back(), "Failed to cast src operand to ASM::OperandNode");
    buffer_.pop_back();
    node.dst_->accept(*this);
    auto converted_dst = As<ASM::OperandNode>(
        buffer_.back(), "Failed to cast dst operand to ASM::OperandNode");
    buffer_.pop_back();

    // actual logic
    buffer_.push_back(std::make_shared<ASM::MovNode>(converted_src, converted_dst));
    buffer_.push_back(std::make_shared<ASM::NotNode>(converted_dst));
}

void TackyToAsmVisitor::visit(Tacky::NegateNode& node) {
    // todo: move this boilerplate to parent unary class
    node.src_->accept(*this);
    auto converted_src = As<ASM::OperandNode>(
        buffer_.back(), "Failed to cast src operand to ASM::OperandNode");
    buffer_.pop_back();
    node.dst_->accept(*this);
    auto converted_dst = As<ASM::OperandNode>(
        buffer_.back(), "Failed to cast dst operand to ASM::OperandNode");
    buffer_.pop_back();

    // actual logic
    buffer_.push_back(std::make_shared<ASM::MovNode>(converted_src, converted_dst));
    buffer_.push_back(std::make_shared<ASM::NegNode>(converted_dst));
}

void TackyToAsmVisitor::visit(Tacky::NotNode& node) {}

void TackyToAsmVisitor::visit(Tacky::DivNode& node) {
    node.left_->accept(*this);
    auto left_operand = As<ASM::OperandNode>(
        buffer_.back(), "Failed to cast left operand in div to ASM::OperandNode");
    buffer_.pop_back();

    node.right_->accept(*this);
    auto right_operand = As<ASM::OperandNode>(
        buffer_.back(), "Failed to cast right operand in div to ASM::OperandNode");
    buffer_.pop_back();

    node.dst_->accept(*this);
    auto dst_operand = As<ASM::OperandNode>(
        buffer_.back(), "Failed to cast dst operand in div to ASM::OperandNode");
    buffer_.pop_back();

    buffer_.push_back(std::make_shared<ASM::MovNode>(
        left_operand,
        std::make_shared<ASM::RegisterNode>(ASM::Register::AX)
    ));

    buffer_.push_back(std::make_shared<ASM::CDQNode>());

    buffer_.push_back(std::make_shared<ASM::DivNode>(right_operand));
    buffer_.push_back(std::make_shared<ASM::MovNode>(
        std::make_shared<ASM::RegisterNode>(ASM::Register::AX),
        dst_operand
    ));
}

void TackyToAsmVisitor::visit(Tacky::ModNode& node) {
    node.left_->accept(*this);
    auto left_operand = As<ASM::OperandNode>(
        buffer_.back(), "Failed to cast left operand in div to ASM::OperandNode");
    buffer_.pop_back();

    node.right_->accept(*this);
    auto right_operand = As<ASM::OperandNode>(
        buffer_.back(), "Failed to cast right operand in div to ASM::OperandNode");
    buffer_.pop_back();

    node.dst_->accept(*this);
    auto dst_operand = As<ASM::OperandNode>(
        buffer_.back(), "Failed to cast dst operand in div to ASM::OperandNode");
    buffer_.pop_back();

    buffer_.push_back(std::make_shared<ASM::MovNode>(
        left_operand,
        std::make_shared<ASM::RegisterNode>(ASM::Register::AX)
    ));

    buffer_.push_back(std::make_shared<ASM::CDQNode>());

    buffer_.push_back(std::make_shared<ASM::DivNode>(right_operand));
    buffer_.push_back(std::make_shared<ASM::MovNode>(
        std::make_shared<ASM::RegisterNode>(ASM::Register::DX),
        dst_operand
    ));
}

void TackyToAsmVisitor::visit(Tacky::MultNode& node) {
    node.left_->accept(*this);
    auto left_operand = As<ASM::OperandNode>(
        buffer_.back(), "Failed to cast left operand in MULT to ASM::OperandNode");
    buffer_.pop_back();

    node.right_->accept(*this);
    auto right_operand = As<ASM::OperandNode>(
        buffer_.back(), "Failed to cast right operand in MULT to ASM::OperandNode");
    buffer_.pop_back();

    node.dst_->accept(*this);
    auto dst_operand = As<ASM::OperandNode>(
        buffer_.back(), "Failed to cast dst operand in MULT to ASM::OperandNode");
    buffer_.pop_back();

    buffer_.push_back(std::make_shared<ASM::MovNode>(
        left_operand,
        dst_operand
    ));

    buffer_.push_back(std::make_shared<ASM::MultNode>(
        right_operand,
        dst_operand
    ));
}

void TackyToAsmVisitor::visit(Tacky::PlusNode& node) {
    node.left_->accept(*this);
    auto left_operand = As<ASM::OperandNode>(
        buffer_.back(), "Failed to cast left operand in MULT to ASM::OperandNode");
    buffer_.pop_back();

    node.right_->accept(*this);
    auto right_operand = As<ASM::OperandNode>(
        buffer_.back(), "Failed to cast right operand in MULT to ASM::OperandNode");
    buffer_.pop_back();

    node.dst_->accept(*this);
    auto dst_operand = As<ASM::OperandNode>(
        buffer_.back(), "Failed to cast dst operand in MULT to ASM::OperandNode");
    buffer_.pop_back();

    buffer_.push_back(std::make_shared<ASM::MovNode>(
        left_operand,
        dst_operand
    ));

    buffer_.push_back(std::make_shared<ASM::AddNode>(
        right_operand,
        dst_operand
    ));
}

void TackyToAsmVisitor::visit(Tacky::MinusNode& node) {
    node.left_->accept(*this);
    auto left_operand = As<ASM::OperandNode>(
        buffer_.back(), "Failed to cast left operand in MULT to ASM::OperandNode");
    buffer_.pop_back();

    node.right_->accept(*this);
    auto right_operand = As<ASM::OperandNode>(
        buffer_.back(), "Failed to cast right operand in MULT to ASM::OperandNode");
    buffer_.pop_back();

    node.dst_->accept(*this);
    auto dst_operand = As<ASM::OperandNode>(
        buffer_.back(), "Failed to cast dst operand in MULT to ASM::OperandNode");
    buffer_.pop_back();

    buffer_.push_back(std::make_shared<ASM::MovNode>(
        left_operand,
        dst_operand
    ));

    buffer_.push_back(std::make_shared<ASM::SubNode>(
        right_operand,
        dst_operand
    ));
}

void TackyToAsmVisitor::visit(Tacky::AndNode& node) {}
void TackyToAsmVisitor::visit(Tacky::OrNode& node) {}

void TackyToAsmVisitor::visit(Tacky::BitwiseAndNode& node) {
    node.left_->accept(*this);
    auto left_operand = As<ASM::OperandNode>(
        buffer_.back(), "Failed to cast left operand in BitwiseAndNode to ASM::OperandNode");
    buffer_.pop_back();

    node.right_->accept(*this);
    auto right_operand = As<ASM::OperandNode>(
        buffer_.back(), "Failed to cast right operand in BitwiseAndNode to ASM::OperandNode");
    buffer_.pop_back();

    node.dst_->accept(*this);
    auto dst_operand = As<ASM::OperandNode>(
        buffer_.back(), "Failed to cast dst operand in BitwiseAndNode to ASM::OperandNode");
    buffer_.pop_back();

    buffer_.push_back(std::make_shared<ASM::MovNode>(
        left_operand,
        dst_operand
    ));

    buffer_.push_back(std::make_shared<ASM::BitwiseAndNode>(
        right_operand,
        dst_operand
    ));
}

void TackyToAsmVisitor::visit(Tacky::BitwiseOrNode& node) {
    node.left_->accept(*this);
    auto left_operand = As<ASM::OperandNode>(
        buffer_.back(), "Failed to cast left operand in BitwiseAndNode to ASM::OperandNode");
    buffer_.pop_back();

    node.right_->accept(*this);
    auto right_operand = As<ASM::OperandNode>(
        buffer_.back(), "Failed to cast right operand in BitwiseAndNode to ASM::OperandNode");
    buffer_.pop_back();

    node.dst_->accept(*this);
    auto dst_operand = As<ASM::OperandNode>(
        buffer_.back(), "Failed to cast dst operand in BitwiseAndNode to ASM::OperandNode");
    buffer_.pop_back();

    buffer_.push_back(std::make_shared<ASM::MovNode>(
        left_operand,
        dst_operand
    ));

    buffer_.push_back(std::make_shared<ASM::BitwiseOrNode>(
        right_operand,
        dst_operand
    ));
}

void TackyToAsmVisitor::visit(Tacky::BitwiseXorNode& node) {
    node.left_->accept(*this);
    auto left_operand = As<ASM::OperandNode>(
        buffer_.back(), "Failed to cast left operand in BitwiseXorNode to ASM::OperandNode");
    buffer_.pop_back();

    node.right_->accept(*this);
    auto right_operand = As<ASM::OperandNode>(
        buffer_.back(), "Failed to cast right operand in BitwiseXorNode to ASM::OperandNode");
    buffer_.pop_back();

    node.dst_->accept(*this);
    auto dst_operand = As<ASM::OperandNode>(
        buffer_.back(), "Failed to cast dst operand in BitwiseXorNode to ASM::OperandNode");
    buffer_.pop_back();

    buffer_.push_back(std::make_shared<ASM::MovNode>(
        left_operand,
        dst_operand
    ));

    buffer_.push_back(std::make_shared<ASM::BitwiseXorNode>(
        right_operand,
        dst_operand
    ));
}

void TackyToAsmVisitor::visit(Tacky::BitwiseLeftShiftNode& node) {
    node.left_->accept(*this);
    auto left_operand = As<ASM::OperandNode>(
        buffer_.back(), "Failed to cast left operand in BitwiseXorNode to ASM::OperandNode");
    buffer_.pop_back();

    node.right_->accept(*this);
    auto right_operand = As<ASM::OperandNode>(
        buffer_.back(), "Failed to cast right operand in BitwiseXorNode to ASM::OperandNode");
    buffer_.pop_back();

    node.dst_->accept(*this);
    auto dst_operand = As<ASM::OperandNode>(
        buffer_.back(), "Failed to cast dst operand in BitwiseXorNode to ASM::OperandNode");
    buffer_.pop_back();

    buffer_.push_back(std::make_shared<ASM::MovNode>(
        left_operand,
        dst_operand
    ));

    buffer_.push_back(std::make_shared<ASM::SalNode>(
        right_operand,
        dst_operand
    ));
}

void TackyToAsmVisitor::visit(Tacky::BitwiseRightShiftNode& node) {
    node.left_->accept(*this);
    auto left_operand = As<ASM::OperandNode>(
        buffer_.back(), "Failed to cast left operand in BitwiseXorNode to ASM::OperandNode");
    buffer_.pop_back();

    node.right_->accept(*this);
    auto right_operand = As<ASM::OperandNode>(
        buffer_.back(), "Failed to cast right operand in BitwiseXorNode to ASM::OperandNode");
    buffer_.pop_back();

    node.dst_->accept(*this);
    auto dst_operand = As<ASM::OperandNode>(
        buffer_.back(), "Failed to cast dst operand in BitwiseXorNode to ASM::OperandNode");
    buffer_.pop_back();

    buffer_.push_back(std::make_shared<ASM::MovNode>(
        left_operand,
        dst_operand
    ));

    buffer_.push_back(std::make_shared<ASM::SarNode>(
        right_operand,
        dst_operand
    ));
}

void TackyToAsmVisitor::visit(Tacky::IntegerNode& node) {
    buffer_.push_back(std::make_shared<ASM::ImmNode>(node.value_));
}

void TackyToAsmVisitor::visit(Tacky::VariableNode& node) {
    buffer_.push_back(std::make_shared<ASM::PseudoNode>(node.name_));
}


} //namespace Codegen