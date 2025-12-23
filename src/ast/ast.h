#include <vector>
#include <list>
#include <string>
#include "src/ast/type.h"
#include <variant>
#include <memory>

#pragma once

class Visitor;
struct TypeNode;
struct FunctionArgumentsNode;
struct StatementNode;
struct ReturnStatementNode;
struct ExpressionNode;
struct ConstantValueNode;
struct IntegerValueNode;
struct UnaryExpressionNode;
struct TildeUnaryExpressionNode;
struct MinusUnaryExpressionNode;
struct StatementBlockNode;
struct FunctionNode;
struct ProgramNode;

// TODO: make nodes reference each other via unique pointers
// so we can have polymorphism.

class Visitor {
public:
    virtual ~Visitor() = default;
    virtual void visit(TypeNode& node) = 0;
    virtual void visit(FunctionArgumentsNode& node) = 0;
    virtual void visit(ReturnStatementNode& node) = 0;
    virtual void visit(StatementBlockNode& node) = 0;
    virtual void visit(FunctionNode& node) = 0;
    virtual void visit(ProgramNode& node) = 0;
    virtual void visit(TildeUnaryExpressionNode& node) = 0;
    virtual void visit(MinusUnaryExpressionNode& node) = 0;
    virtual void visit(IntegerValueNode& node) = 0;
};

class ASTNode {
public:
    virtual ~ASTNode() = 0;
    virtual void accept(Visitor& v) = 0;
};

class ExpressionNode : public ASTNode {
public:
    virtual ~ExpressionNode() = 0;
    virtual void accept(Visitor& v) = 0;
};

class ConstantValueNode : public ExpressionNode {
public:
    virtual ~ConstantValueNode() = 0;
    virtual void accept(Visitor& v) = 0;
};

class IntegerValueNode : public ConstantValueNode {
public:
    int value_;
    IntegerValueNode(int value) : value_(value) {}
    void accept(Visitor& v) override {
        v.visit(*this);
    }
    ~IntegerValueNode() override = default;
};

class UnaryExpressionNode : public ExpressionNode {
public:
    virtual ~UnaryExpressionNode() = 0;
    virtual void accept(Visitor& v) = 0;
};

class TildeUnaryExpressionNode : public UnaryExpressionNode {
public:
    std::shared_ptr<ExpressionNode> operand_;
    TildeUnaryExpressionNode(std::shared_ptr<ExpressionNode> operand) : operand_(std::move(operand)) {}
    void accept(Visitor& v) override {
        v.visit(*this);
    }
    ~TildeUnaryExpressionNode() override = default;
};

class MinusUnaryExpressionNode : public UnaryExpressionNode {
public:
    std::shared_ptr<ExpressionNode> operand_;
    MinusUnaryExpressionNode(std::shared_ptr<ExpressionNode> operand) : operand_(std::move(operand)) {}
    void accept(Visitor& v) override {
        v.visit(*this);
    }
    ~MinusUnaryExpressionNode() override = default;
};

struct FunctionArgument {
    Type type;
    std::string name;
};


struct TypeNode: public ASTNode {
public:
    Type type_;
    void accept(Visitor& v) override {
        v.visit(*this);
    }
    TypeNode(Type type_): type_(type_) {}
};


struct FunctionArgumentsNode: public ASTNode {
public:
    std::list<FunctionArgument> arguments_;
    void accept(Visitor& v) override {
        v.visit(*this);
    }
    FunctionArgumentsNode() = default;
    FunctionArgumentsNode(std::list<FunctionArgument> &&arguments) : arguments_(std::move(arguments)) {}
};

struct StatementNode: public ASTNode {
public:
    virtual ~StatementNode() = 0;
    void accept(Visitor& v) = 0;
};

struct ReturnStatementNode: public StatementNode {
public:
    Type type_;
    std::shared_ptr<ExpressionNode> return_value_;
    ReturnStatementNode(Type return_type, std::shared_ptr<ExpressionNode> return_value)
        : type_(return_type), return_value_(std::move(return_value)) {}
    void accept(Visitor& v) override {
        v.visit(*this);
    }
    ~ReturnStatementNode() override = default;
};

struct StatementBlockNode: public ASTNode {
public:
    std::vector<std::shared_ptr<StatementNode>> statements_;
    StatementBlockNode() = default;
    StatementBlockNode(std::vector<std::shared_ptr<StatementNode>> statements) : statements_(std::move(statements)) {}
    void accept(Visitor& v) override {
        v.visit(*this);
    }
};

struct FunctionNode: public ASTNode {
public:
    std::string name_;
    std::shared_ptr<TypeNode> type_node_;
    std::shared_ptr<FunctionArgumentsNode> arguments_node_;
    std::shared_ptr<StatementBlockNode> body_;
    FunctionNode(std::string name, std::shared_ptr<TypeNode> type_node, std::shared_ptr<FunctionArgumentsNode> arguments, std::shared_ptr<StatementBlockNode> body)
        : name_(std::move(name)), type_node_(std::move(type_node)), arguments_node_(std::move(arguments)), body_(std::move(body)) {}
    void accept(Visitor& v) override {
        v.visit(*this);
    }
};

struct ProgramNode: public ASTNode {
public:
    std::vector<std::shared_ptr<FunctionNode>> functions_;
    ProgramNode() = default;
    ProgramNode(std::vector<std::shared_ptr<FunctionNode>> functions) : functions_(std::move(functions)) {}
    void accept(Visitor& v) override {
        v.visit(*this);
    }
};

ASTNode::~ASTNode() {}
ExpressionNode::~ExpressionNode() {}
ConstantValueNode::~ConstantValueNode() {}
UnaryExpressionNode::~UnaryExpressionNode() {}
StatementNode::~StatementNode() {}