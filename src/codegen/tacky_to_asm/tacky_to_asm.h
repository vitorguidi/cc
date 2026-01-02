#ifndef TACKY_TO_ASM_VISITOR_H
#define TACKY_TO_ASM_VISITOR_H

#include "src/tacky/tacky.h"
#include "src/asm/asm_ast.h"
#include <stdexcept>
#include <unordered_map>
#include <deque>

namespace Codegen {

template<typename T>
inline std::shared_ptr<T> As(std::shared_ptr<ASM::AstNode>& node, const std::string& error_msg) {
    auto casted_ptr = std::dynamic_pointer_cast<T>(node);
    if (!casted_ptr) {
        throw std::runtime_error(error_msg);
    }
    return casted_ptr;
}

class TackyToAsmVisitor : public Tacky::Visitor {
public:
    ~TackyToAsmVisitor() = default;
    TackyToAsmVisitor() = default;
    void visit(Tacky::ProgramNode& node) override;
    void visit(Tacky::FunctionNode& node) override;
    void visit(Tacky::ReturnNode& node) override;
    void visit(Tacky::ComplementNode& node) override;
    void visit(Tacky::NegateNode& node) override;
    void visit(Tacky::IntegerNode& node) override;
    void visit(Tacky::VariableNode& node) override;
    void visit(Tacky::DivNode& node) override;
    void visit(Tacky::ModNode& node) override;
    void visit(Tacky::MultNode& node) override;
    void visit(Tacky::PlusNode& node) override;
    void visit(Tacky::MinusNode& node) override;
    std::shared_ptr<ASM::ProgramNode> get_asm_from_tacky(std::shared_ptr<Tacky::ProgramNode> tacky_program);
    std::vector<std::shared_ptr<ASM::AstNode>> buffer_;
};

class ASMRewriteVisitor : public ASM::Visitor {
public:
    ~ASMRewriteVisitor() = default;
    ASMRewriteVisitor() = default;
    void visit(ASM::ProgramNode& node) override;
    void visit(ASM::FunctionNode& node) override;
    void visit(ASM::NegNode& node) override;
    void visit(ASM::NotNode& node) override;
    void visit(ASM::MovNode& node) override;
    void visit(ASM::RetNode& node) override;
    void visit(ASM::AllocateStackNode& node) override;
    void visit(ASM::ImmNode& node) override;
    void visit(ASM::StackNode& node) override;
    void visit(ASM::RegisterNode& node) override;
    void visit(ASM::PseudoNode& node) override;
    void visit(ASM::MultNode& node) override;
    void visit(ASM::DivNode& node) override;
    void visit(ASM::AddNode& node) override;
    void visit(ASM::SubNode& node) override;
    void visit(ASM::CDQNode& node) override;
    std::shared_ptr<ASM::ProgramNode> get_rewritten_asm_program(std::shared_ptr<ASM::ProgramNode> program);
    std::deque<std::shared_ptr<ASM::AstNode>> buffer_;
};

class PseudoReplacerVisitor : public ASMRewriteVisitor {
public:
    ~PseudoReplacerVisitor() = default;
    PseudoReplacerVisitor() : current_offset_(0) {}
    void visit(ASM::PseudoNode& node) override;
    int get_offset();
    int current_offset_;
    std::unordered_map<std::string, int> stack_offsets_;
};

class InstructionFixUpVisitor : public ASMRewriteVisitor {
public:
    ~InstructionFixUpVisitor() = default;
    InstructionFixUpVisitor(int offset)
        : ASMRewriteVisitor(), max_stack_offset_(offset) {}
    void visit(ASM::FunctionNode& node) override;
    void visit(ASM::MovNode& node) override;
    void visit(ASM::DivNode& node) override;
    void visit(ASM::MultNode& node) override;
    void visit(ASM::AddNode& node) override;
    void visit(ASM::SubNode& node) override;
    int max_stack_offset_;
};

} // namespace Codegen

#endif // TACKY_TO_ASM_VISITOR_H