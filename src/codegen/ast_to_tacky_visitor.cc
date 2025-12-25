#include "src/codegen/ast_to_tacky_visitor.h"
#include "src/tacky/tacky.h"
#include <memory>
#include <algorithm>

namespace Codegen {

template<typename T>
std::shared_ptr<T> AstToTackyVisitor::get_result()
{
    if (result_buffer_.empty()) {
        throw std::runtime_error("Result buffer is empty when fetching result");
    }
    auto ast_node = result_buffer_.back();
    result_buffer_.pop_back();
    auto casted_node = std::dynamic_pointer_cast<T>(ast_node);
    if (!casted_node) {
        throw std::runtime_error("Failed to cast AST node in result buffer");
    }
    return casted_node;
}

template <typename T>
std::vector<std::shared_ptr<T>> AstToTackyVisitor::get_results() {
    std::vector<std::shared_ptr<T>> results;
    while (!result_buffer_.empty()) {
        auto back_node = get_result<T>();
        results.push_back(back_node);
    }
    std::reverse(results.begin(), results.end());
    return results;
}

std::shared_ptr<Tacky::ProgramNode> AstToTackyVisitor::get_tacky_from_c_ast(std::shared_ptr<CAst::ProgramNode> root_node)
{
    if (!result_buffer_.empty()) {
        throw std::runtime_error("Result buffer not empty at start of conversion");
    }
    root_node->accept(*this);
    if (result_buffer_.size() != 1) {
        throw std::runtime_error("Expected exactly one Tacky AST root node");
    }
    auto result_node = std::dynamic_pointer_cast<Tacky::ProgramNode>(result_buffer_.back());
    result_buffer_.pop_back();
    if (!result_node) {
        throw std::runtime_error("Expected Tacky ProgramNode as conversion result");
    }
    return result_node;
}

void AstToTackyVisitor::visit(CAst::ProgramNode& node) {
    for (auto& function : node.functions_) {
        function->accept(*this);
    }
    auto functions = get_results<Tacky::FunctionNode>();
    auto result = std::make_shared<Tacky::ProgramNode>(std::move(functions) );
    result_buffer_.push_back(result);
}

void AstToTackyVisitor::visit(CAst::FunctionNode& node) {
    for (auto& statement : node.body_->statements_) {
        statement->accept(*this);
    }

    auto tacky_instructions = get_results<Tacky::InstructionNode>();

    auto function_result = std::make_shared<Tacky::FunctionNode>(
        node.name_,
        tacky_instructions
    );

    result_buffer_.push_back(function_result);
}

void AstToTackyVisitor::visit(CAst::ReturnStatementNode& node) {
    if (node.type_ != CAst::Type::INTEGER) {
       throw std::runtime_error("Only integer return types are supported for now");
    }

    // recursively parses an expression and returns a value node
    node.return_value_->accept(*this);

    // if we end up on a leaf value node, we can just create a return node
    // leaf visitors push the value onto the stack

    result_buffer_.push_back(
        std::make_shared<Tacky::ReturnNode>(get_result<Tacky::ValueNode>())
    );

}

// clean way: make visit return an optional for the location where the result is stored

void AstToTackyVisitor::visit(CAst::TildeUnaryExpressionNode& node) {

    node.operand_->accept(*this);

    auto inner_expression_result = get_result<Tacky::ValueNode>();
    auto dst = generate_temp_var_name();

    auto tacky_op = std::make_shared<Tacky::NegateNode>(
        inner_expression_result,
        std::make_shared<Tacky::VariableNode>(dst)
    );

    auto value_return_hint = std::make_shared<Tacky::VariableNode>(dst);
    
    result_buffer_.push_back(tacky_op);
    result_buffer_.push_back(value_return_hint);
}

void AstToTackyVisitor::visit(CAst::MinusUnaryExpressionNode& node) {
    node.operand_->accept(*this);

    // this fials to cast 
    auto inner_expression_result = get_result<Tacky::ValueNode>();

    auto dst = generate_temp_var_name();

    auto tacky_op = std::make_shared<Tacky::ComplementNode>(
        inner_expression_result,
        std::make_shared<Tacky::VariableNode>(dst)
    );

    auto value_return_hint = std::make_shared<Tacky::VariableNode>(dst);
    
    result_buffer_.push_back(tacky_op);
    result_buffer_.push_back(value_return_hint);
}

void AstToTackyVisitor::visit(CAst::IntegerValueNode& node) {
    result_buffer_.push_back(
        std::make_shared<Tacky::IntegerNode>(node.value_)
    );
}

void AstToTackyVisitor::visit(CAst::TypeNode& node) {}

void AstToTackyVisitor::visit(CAst::FunctionArgumentsNode& node) {}

void AstToTackyVisitor::visit(CAst::StatementBlockNode& node) {}

std::string AstToTackyVisitor::generate_temp_var_name() {
    return "_tacky_temp_" + std::to_string(temp_var_counter_++);
}

} // namespace Codegen