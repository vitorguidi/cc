#pragma once

#include "src/ast/ast.h"

class AstToAsmVisitor: public Visitor {
public:
    AstToAsmVisitor() = default;
    ~AstToAsmVisitor() = default;
    void visit(ProgramNode& node) override;
    void visit(FunctionNode& node) override;
    void visit(TypeNode& node) override;
    void visit(FunctionArgumentsNode& node) override;
    void visit(StatementBlockNode& node) override;
    void visit(ReturnStatementNode& node) override;
    void visit(TildeUnaryExpressionNode& node) override;
    void visit(MinusUnaryExpressionNode& node) override;
    void visit(IntegerValueNode& node) override;
    const std::vector<std::string>& get_assembly_output() const;
private:
    std::vector<std::string> assembly_output_;
};
