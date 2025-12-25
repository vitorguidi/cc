#include "src/ast/ast.h"
#include "src/codegen/ast_to_asm_visitor.h"
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <variant>

namespace Codegen {

const std::vector<std::string>& AstToAsmVisitor::get_assembly_output() const {
    return assembly_output_;
}

void AstToAsmVisitor::visit(CAst::ProgramNode& node) {
    for (auto& function : node.functions_) {
        function->accept(*this);
    }
    assembly_output_.push_back("\t.section .note.GNU-stack,\"\",@progbits");
}
void AstToAsmVisitor::visit(CAst::FunctionNode& node) {
    assembly_output_.push_back("\t.globl " + node.name_);
    assembly_output_.push_back(node.name_ + ":");
    node.body_->accept(*this);
}
void AstToAsmVisitor::visit(CAst::TypeNode& node) {}
void AstToAsmVisitor::visit(CAst::FunctionArgumentsNode& node) {}

void AstToAsmVisitor::visit(CAst::StatementBlockNode& node) {
    for (auto& statement : node.statements_) {
        statement->accept(*this);
    }
}
void AstToAsmVisitor::visit(CAst::ReturnStatementNode& node) {
    if (node.type_ != CAst::Type::INTEGER) {
        throw std::runtime_error("Unsupported return type in code generation");
    }
    node.return_value_->accept(*this);
}

void AstToAsmVisitor::visit(CAst::IntegerValueNode& node) {
    assembly_output_.push_back("\tmovl $" + std::to_string(node.value_) + ", %eax");
    assembly_output_.push_back("\tret");
}

void AstToAsmVisitor::visit(CAst::TildeUnaryExpressionNode& node) {}
void AstToAsmVisitor::visit(CAst::MinusUnaryExpressionNode& node) {}

} // namespace Codegen