#pragma once

#include "src/ast/ast.h"

namespace Codegen {

class AstToAsmVisitor: public CAst::Visitor {
public:
    AstToAsmVisitor() = default;
    ~AstToAsmVisitor() = default;
    void visit(CAst::ProgramNode& node) override;
    void visit(CAst::FunctionNode& node) override;
    void visit(CAst::TypeNode& node) override;
    void visit(CAst::FunctionArgumentsNode& node) override;
    void visit(CAst::StatementBlockNode& node) override;
    void visit(CAst::ReturnStatementNode& node) override;
    void visit(CAst::TildeUnaryExpressionNode& node) override;
    void visit(CAst::MinusUnaryExpressionNode& node) override;
    void visit(CAst::IntegerValueNode& node) override;
    const std::vector<std::string>& get_assembly_output() const;
private:
    std::vector<std::string> assembly_output_;
};

} //namespace Codegen