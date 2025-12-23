#include "src/ast/ast.h"

// --- Pure Virtual Destructors ---
// Even though they are pure virtual, they MUST have a body in the .cpp file.
ASTNode::~ASTNode() {}
ExpressionNode::~ExpressionNode() {}
StatementNode::~StatementNode() {}
ConstantValueNode::~ConstantValueNode() {}
UnaryExpressionNode::~UnaryExpressionNode() {}

// --- IntegerValueNode ---
IntegerValueNode::IntegerValueNode(int value) : value_(value) {}
void IntegerValueNode::accept(Visitor& v) { v.visit(*this); }

// --- Unary Expressions ---
TildeUnaryExpressionNode::TildeUnaryExpressionNode(std::shared_ptr<ExpressionNode> operand) 
    : operand_(std::move(operand)) {}
void TildeUnaryExpressionNode::accept(Visitor& v) { v.visit(*this); }

MinusUnaryExpressionNode::MinusUnaryExpressionNode(std::shared_ptr<ExpressionNode> operand) 
    : operand_(std::move(operand)) {}
void MinusUnaryExpressionNode::accept(Visitor& v) { v.visit(*this); }

// --- Structural Nodes ---
TypeNode::TypeNode(Type type) : type_(type) {}
void TypeNode::accept(Visitor& v) { v.visit(*this); }

FunctionArgumentsNode::FunctionArgumentsNode(std::list<FunctionArgument>&& arguments) 
    : arguments_(std::move(arguments)) {}
void FunctionArgumentsNode::accept(Visitor& v) { v.visit(*this); }

ReturnStatementNode::ReturnStatementNode(Type return_type, std::shared_ptr<ExpressionNode> return_value)
    : type_(return_type), return_value_(std::move(return_value)) {}
void ReturnStatementNode::accept(Visitor& v) { v.visit(*this); }

StatementBlockNode::StatementBlockNode(std::vector<std::shared_ptr<StatementNode>> statements) 
    : statements_(std::move(statements)) {}
void StatementBlockNode::accept(Visitor& v) { v.visit(*this); }

FunctionNode::FunctionNode(std::string name, std::shared_ptr<TypeNode> type_node, 
                           std::shared_ptr<FunctionArgumentsNode> arguments, 
                           std::shared_ptr<StatementBlockNode> body)
    : name_(std::move(name)), type_node_(std::move(type_node)), 
      arguments_node_(std::move(arguments)), body_(std::move(body)) {}
void FunctionNode::accept(Visitor& v) { v.visit(*this); }

ProgramNode::ProgramNode(std::vector<std::shared_ptr<FunctionNode>> functions) 
    : functions_(std::move(functions)) {}
void ProgramNode::accept(Visitor& v) { v.visit(*this); }