#ifndef TACKY_TO_ASM_VISITOR_H
#define TACKY_TO_ASM_VISITOR_H

#include "src/tacky/tacky.h"
#include "src/asm/asm_ast.h"
#include <stdexcept>

namespace Codegen {

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
    std::shared_ptr<ASM::ProgramNode> get_asm_from_tacky(std::shared_ptr<Tacky::ProgramNode> tacky_program);
    std::vector<std::shared_ptr<ASM::AstNode>> buffer_;
};

} // namespace Codegen

#endif // TACKY_TO_ASM_VISITOR_H