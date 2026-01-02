#pragma once

#include <vector>
#include <list>
#include <string>
#include <memory>
#include "src/ast/type.h"

namespace CAst {

// Forward declarations
class Visitor;
struct TypeNode;
struct FunctionArgumentsNode;
struct ReturnStatementNode;
struct StatementBlockNode;
struct FunctionNode;
struct ProgramNode;
struct IntegerValueNode;
struct TildeUnaryExpressionNode;
struct MinusUnaryExpressionNode;
struct DivNode;
struct ModNode;
struct MultNode;
struct PlusNode;
struct MinusNode;

// --- Visitor Interface ---
class Visitor {
public:
    virtual ~Visitor() = default;
    virtual void visit(TypeNode& node) = 0;
    virtual void visit(FunctionArgumentsNode& node) = 0;
    virtual void visit(ReturnStatementNode& node) = 0;
    virtual void visit(StatementBlockNode& node) = 0;
    virtual void visit(FunctionNode& node) = 0;
    virtual void visit(TildeUnaryExpressionNode& node) = 0;
    virtual void visit(MinusUnaryExpressionNode& node) = 0;
    virtual void visit(IntegerValueNode& node) = 0;
    virtual void visit(ModNode& node) = 0;
    virtual void visit(DivNode& node) = 0;
    virtual void visit(MultNode& node) = 0;
    virtual void visit(PlusNode& node) = 0;
    virtual void visit(MinusNode& node) = 0;
    virtual void visit(ProgramNode& node) = 0;
};

// --- Base Nodes ---
class ASTNode {
public:
    virtual ~ASTNode() = 0;
    virtual void accept(Visitor& v) = 0;
};

class ExpressionNode : public ASTNode {
public:
    virtual ~ExpressionNode() = 0;
    virtual void accept(Visitor& v) override = 0;
};

class StatementNode : public ASTNode {
public:
    virtual ~StatementNode() = 0;
    virtual void accept(Visitor& v) override = 0;
};

// --- Expressions ---
class ConstantValueNode : public ExpressionNode {
public:
    virtual ~ConstantValueNode() = 0;
};

class IntegerValueNode : public ConstantValueNode {
public:
    int value_;
    IntegerValueNode(int value);
    void accept(Visitor& v) override;
};

class UnaryExpressionNode : public ExpressionNode {
public:
    virtual ~UnaryExpressionNode() = 0;
};

class TildeUnaryExpressionNode : public UnaryExpressionNode {
public:
    std::shared_ptr<ExpressionNode> operand_;
    TildeUnaryExpressionNode(std::shared_ptr<ExpressionNode> operand);
    void accept(Visitor& v) override;
};

class MinusUnaryExpressionNode : public UnaryExpressionNode {
public:
    std::shared_ptr<ExpressionNode> operand_;
    MinusUnaryExpressionNode(std::shared_ptr<ExpressionNode> operand);
    void accept(Visitor& v) override;
};

class BinaryExpressionNode : public ExpressionNode {
public:
    std::shared_ptr<ExpressionNode> left_, right_;
    BinaryExpressionNode(std::shared_ptr<ExpressionNode> left, std::shared_ptr<ExpressionNode> right)
        :   left_(left), right_(right) {}
    virtual void accept(Visitor& v) = 0;
    virtual ~BinaryExpressionNode() = default;
};

class ModNode : public BinaryExpressionNode {
public:
    ModNode(std::shared_ptr<ExpressionNode> left, std::shared_ptr<ExpressionNode> right)
        :   BinaryExpressionNode(left, right) {}
    ~ModNode() = default;
    void accept(Visitor& v) override;
};

class DivNode : public BinaryExpressionNode {
public:
    DivNode(std::shared_ptr<ExpressionNode> left, std::shared_ptr<ExpressionNode> right)
        :   BinaryExpressionNode(left, right) {}
    ~DivNode() = default;
    void accept(Visitor& v) override;
};

class MultNode : public BinaryExpressionNode {
public:
    MultNode(std::shared_ptr<ExpressionNode> left, std::shared_ptr<ExpressionNode> right)
        :   BinaryExpressionNode(left, right) {}
    ~MultNode() = default;
    void accept(Visitor& v) override;
};

class PlusNode : public BinaryExpressionNode {
public:
    PlusNode(std::shared_ptr<ExpressionNode> left, std::shared_ptr<ExpressionNode> right)
        :   BinaryExpressionNode(left, right) {}
    ~PlusNode() = default;
    void accept(Visitor& v) override;
};

class MinusNode : public BinaryExpressionNode {
public:
    MinusNode(std::shared_ptr<ExpressionNode> left, std::shared_ptr<ExpressionNode> right)
        :   BinaryExpressionNode(left, right) {}
    ~MinusNode() = default;
    void accept(Visitor& v) override;
};

// --- Structural Nodes ---
struct FunctionArgument {
    Type type;
    std::string name;
};

struct TypeNode : public ASTNode {
    Type type_;
    TypeNode(Type type);
    void accept(Visitor& v) override;
};

struct FunctionArgumentsNode : public ASTNode {
    std::list<FunctionArgument> arguments_;
    FunctionArgumentsNode() = default;
    FunctionArgumentsNode(std::list<FunctionArgument>&& arguments);
    void accept(Visitor& v) override;
};

struct ReturnStatementNode : public StatementNode {
    Type type_;
    std::shared_ptr<ExpressionNode> return_value_;
    ReturnStatementNode(Type return_type, std::shared_ptr<ExpressionNode> return_value);
    void accept(Visitor& v) override;
};

struct StatementBlockNode : public ASTNode {
    std::vector<std::shared_ptr<StatementNode>> statements_;
    StatementBlockNode() = default;
    StatementBlockNode(std::vector<std::shared_ptr<StatementNode>> statements);
    void accept(Visitor& v) override;
};

struct FunctionNode : public ASTNode {
    std::string name_;
    std::shared_ptr<TypeNode> type_node_;
    std::shared_ptr<FunctionArgumentsNode> arguments_node_;
    std::shared_ptr<StatementBlockNode> body_;

    FunctionNode(std::string name, 
                 std::shared_ptr<TypeNode> type_node, 
                 std::shared_ptr<FunctionArgumentsNode> arguments, 
                 std::shared_ptr<StatementBlockNode> body);
    void accept(Visitor& v) override;
};

struct ProgramNode : public ASTNode {
    std::vector<std::shared_ptr<FunctionNode>> functions_;
    ProgramNode() = default;
    ProgramNode(std::vector<std::shared_ptr<FunctionNode>> functions);
    void accept(Visitor& v) override;
};

} // namespace CAst