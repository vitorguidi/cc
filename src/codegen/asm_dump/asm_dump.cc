#include "src/codegen/asm_dump/asm_dump.h"

namespace Codegen {

ASMDumper::ASMDumper(std::string filename) : target_filename_(filename) {
    of = std::ofstream(filename);
    if (!of.is_open()) {
        throw std::runtime_error("Unable to open file for assembly output");
    }
}

ASMDumper::~ASMDumper() {
    if (of.is_open()) {
        of.close();
    }
}


void ASMDumper::visit(ASM::ProgramNode& node) {
    of << "\t.section .note.GNU-stack,\"\",@progbits\n";
    of << "\t.text\n";
    for(auto& fn : node.functions_) {
        fn->accept(*this);
    }
}

void ASMDumper::visit(ASM::FunctionNode& node) {
    of << "\t.globl " + node.name_ + "\n";
    of << node.name_ + ":\n";
    of << "\tpushq  %rbp\n";
    of << "\tmovq   %rsp, %rbp\n";
    for(auto& instruction : node.instructions_) {
        instruction->accept(*this);
    }
}

void ASMDumper::visit(ASM::NegNode& node) {
    node.src_->accept(*this);
    auto operand_as_str = buffer_.back();
    buffer_.pop_back();
    of << "\tnegl   " + operand_as_str + "\n";
}

void ASMDumper::visit(ASM::NotNode& node) {
    node.src_->accept(*this);
    auto src_as_str = buffer_.back();
    buffer_.pop_back();
    of << "\tnotl   " + src_as_str + "\n";
}

void ASMDumper::visit(ASM::MovNode& node) {
    node.src_->accept(*this);
    auto src_as_str = buffer_.back();
    buffer_.pop_back();
    node.dst_->accept(*this);
    auto dst_as_str = buffer_.back();
    buffer_.pop_back();    
    of << "\tmovl   " + src_as_str + ",  " + dst_as_str + "\n";
}

void ASMDumper::visit(ASM::RetNode& node) {
    of << "\tmovq   %rbp,%rsp\n";
    of << "\tpopq   %rbp\n";
    of << "\tret\n";
}

void ASMDumper::visit(ASM::AllocateStackNode& node) {
    of << "\tsubq   $" + std::to_string(node.size_) + ", %rsp\n";
}

void ASMDumper::visit(ASM::ImmNode& node) {
    buffer_.push_back(std::string("$") + std::to_string(node.val_));
}

void ASMDumper::visit(ASM::StackNode& node) {
    buffer_.push_back(std::to_string(node.size_) + "(%rbp)");
}

void ASMDumper::visit(ASM::RegisterNode& node) {
    switch (node.reg_) {
        case ASM::Register::AX:
            buffer_.push_back("\%eax");
            break;
        case ASM::Register::DX:
            buffer_.push_back("\%edx");
            break;
        case ASM::Register::R10:
            buffer_.push_back("\%r10d");
            break;
        case ASM::Register::R11:
            buffer_.push_back("\%r11d");
            break;
        default:
            throw std::runtime_error("Unknown register type");
    }
}

void ASMDumper::visit(ASM::DivNode& node) {
    node.src_->accept(*this);
    auto operand = buffer_.back();
    buffer_.pop_back();
    of << "\tidivl  " + operand + "\n";
}

void ASMDumper::visit(ASM::MultNode& node) {
    node.left_->accept(*this);
    auto left = buffer_.back();
    buffer_.pop_back();
    node.right_->accept(*this);
    auto right = buffer_.back();
    buffer_.pop_back();
    of << "\timull  " + left + ",  " + right + "\n";
}

void ASMDumper::visit(ASM::AddNode& node) {
    node.left_->accept(*this);
    auto left = buffer_.back();
    buffer_.pop_back();
    node.right_->accept(*this);
    auto right = buffer_.back();
    buffer_.pop_back();
    of << "\taddl  " + left + ",  " + right + "\n";
}

void ASMDumper::visit(ASM::SubNode& node) {
    node.left_->accept(*this);
    auto left = buffer_.back();
    buffer_.pop_back();
    node.right_->accept(*this);
    auto right = buffer_.back();
    buffer_.pop_back();
    of << "\tsubl  " + left + ",  " + right + "\n";
}

void ASMDumper::visit(ASM::CDQNode& node) {
    of << "\tcdq\n";
}

void ASMDumper::visit(ASM::PseudoNode& node) {
    throw std::runtime_error("Pseudo nodes should have vanished in the first asm pass.");
}

void ASMDumper::dump_assembly(std::shared_ptr<ASM::ProgramNode> asm_program) {
    asm_program->accept(*this);
}

} // namespace Codegen