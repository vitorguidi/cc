#include "src/semantic/semantic.h"

namespace Semantic {

std::string VariableResolutionVisitor::generate_unique_var_name() {
    return "$_var_" +  std::to_string(var_counter_++);
}

void VariableResolutionVisitor::visit(CAst::VariableDeclarationNode& node) {
    auto var_name = node.var_->name_;

    if (lookup_symbol_current_scope(node.var_->name_)) {
        throw std::runtime_error("Variable already declared: " + var_name);
    }

    auto new_name = generate_unique_var_name();
    SymbolEntry new_symbol = SymbolEntry{
        node.var_->name_,
        is_on_file_scope,
        node.type_->type_,
        std::monostate{}
    };
    insert_symbol(node.var_->name_, new_symbol);

    std::optional<std::shared_ptr<CAst::ExpressionNode>> converted_expr = std::nullopt;
    if (node.expr_) {
        node.expr_.value()->accept(*this);
        converted_expr = As<CAst::ExpressionNode>(buffer_.back());
        buffer_.pop_back();
    }
    buffer_.push_back(std::make_shared<CAst::VariableDeclarationNode>(
        std::make_shared<CAst::VariableNode>(new_name),
        node.type_,
        converted_expr
    ));
}

void VariableResolutionVisitor::visit(CAst::VariableNode& node) {
        auto lookup = lookup_symbol(node.name_);
        if(!lookup) throw std::runtime_error("Variable referenced before assignment: " + node.name_);
        buffer_.push_back(std::make_shared<CAst::VariableNode>(lookup.value().name));
}

std::shared_ptr<CAst::ProgramNode> VariableResolutionVisitor::process(CAst::ProgramNode& node) {
    node.accept(*this);
    if (buffer_.size() != 1) {
        throw std::runtime_error("Expected exactly one element in the buffer after semantic analysis.");
    }
    auto program = As<CAst::ProgramNode>(buffer_.back());
    buffer_.pop_back();
    return program;
}

} // namespace Semantic