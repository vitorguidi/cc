#ifndef _AST_H_
#define _AST_H_

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
struct NotUnaryExpressionNode;
struct DivNode;
struct ModNode;
struct MultNode;
struct PlusNode;
struct MinusNode;
struct AndNode;
struct OrNode;
struct BitwiseXorNode;
struct BitwiseAndNode;
struct BitwiseOrNode;
struct BitwiseLeftShiftNode;
struct BitwiseRightShiftNode;

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
    virtual void visit(NotUnaryExpressionNode& node) = 0;
    virtual void visit(AndNode& node) = 0;
    virtual void visit(OrNode& node) = 0;
    virtual void visit(BitwiseXorNode& node) = 0;
    virtual void visit(BitwiseAndNode& node) = 0;
    virtual void visit(BitwiseOrNode& node) = 0;
    virtual void visit(BitwiseLeftShiftNode& node) = 0;
    virtual void visit(BitwiseRightShiftNode& node) = 0;
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
    virtual ~ASTNode() = default;
    virtual void accept(Visitor& v) = 0;
};

class ExpressionNode : public ASTNode {
public:
    virtual ~ExpressionNode() = default;
    virtual void accept(Visitor& v) override = 0;
};

class StatementNode : public ASTNode {
public:
    virtual ~StatementNode() = default;
    virtual void accept(Visitor& v) override = 0;
};

// --- Expressions ---
class ConstantValueNode : public ExpressionNode {
public:
    virtual ~ConstantValueNode() = default;
};

class IntegerValueNode : public ConstantValueNode {
public:
    int value_;
    IntegerValueNode(int value) : value_(value) {}
    void accept(Visitor& v) override {v.visit(*this);}
};

class UnaryExpressionNode : public ExpressionNode {
public:
    std::shared_ptr<ExpressionNode> operand_;
    virtual ~UnaryExpressionNode() = default;
    UnaryExpressionNode(std::shared_ptr<ExpressionNode> operand)
        : operand_(operand) {}

};

class TildeUnaryExpressionNode : public UnaryExpressionNode {
public:
    TildeUnaryExpressionNode(std::shared_ptr<ExpressionNode> operand) 
        : UnaryExpressionNode(operand_) {}
    void accept(Visitor& v) override {v.visit(*this);}
};

class MinusUnaryExpressionNode : public UnaryExpressionNode {
public:
    MinusUnaryExpressionNode(std::shared_ptr<ExpressionNode> operand) 
        : UnaryExpressionNode(operand_) {}
    void accept(Visitor& v) override {v.visit(*this);}
};

class NotUnaryExpressionNode : public UnaryExpressionNode {
public:
    NotUnaryExpressionNode(std::shared_ptr<ExpressionNode> operand)
        : UnaryExpressionNode(operand_) {}
    void accept(Visitor& v) override {v.visit(*this);}
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
    void accept(Visitor& v) override {v.visit(*this);}
};

class DivNode : public BinaryExpressionNode {
public:
    DivNode(std::shared_ptr<ExpressionNode> left, std::shared_ptr<ExpressionNode> right)
        :   BinaryExpressionNode(left, right) {}
    ~DivNode() = default;
    void accept(Visitor& v) override {v.visit(*this);}
};

class MultNode : public BinaryExpressionNode {
public:
    MultNode(std::shared_ptr<ExpressionNode> left, std::shared_ptr<ExpressionNode> right)
        :   BinaryExpressionNode(left, right) {}
    ~MultNode() = default;
    void accept(Visitor& v) override {v.visit(*this);}
};

class PlusNode : public BinaryExpressionNode {
public:
    PlusNode(std::shared_ptr<ExpressionNode> left, std::shared_ptr<ExpressionNode> right)
        :   BinaryExpressionNode(left, right) {}
    ~PlusNode() = default;
    void accept(Visitor& v) override {v.visit(*this);}
};

class MinusNode : public BinaryExpressionNode {
public:
    MinusNode(std::shared_ptr<ExpressionNode> left, std::shared_ptr<ExpressionNode> right)
        :   BinaryExpressionNode(left, right) {}
    ~MinusNode() = default;
    void accept(Visitor& v) override {v.visit(*this);}
};

class AndNode : public BinaryExpressionNode {
public:
    AndNode(std::shared_ptr<ExpressionNode> left, std::shared_ptr<ExpressionNode> right)
        :  BinaryExpressionNode(left, right) {}
    ~AndNode() = default;
    void accept(Visitor& v) override {v.visit(*this);}
};

class BitwiseAndNode : public BinaryExpressionNode {
public:
    BitwiseAndNode(std::shared_ptr<ExpressionNode> left, std::shared_ptr<ExpressionNode> right)
        :  BinaryExpressionNode(left, right) {}
    ~BitwiseAndNode() = default;
    void accept(Visitor& v) override {v.visit(*this);}
};

class OrNode : public BinaryExpressionNode {
public:
    OrNode(std::shared_ptr<ExpressionNode> left, std::shared_ptr<ExpressionNode> right)
        :  BinaryExpressionNode(left, right) {}
    ~OrNode() = default;
    void accept(Visitor& v) override {v.visit(*this);}
};

class BitwiseOrNode : public BinaryExpressionNode {
public:
    BitwiseOrNode(std::shared_ptr<ExpressionNode> left, std::shared_ptr<ExpressionNode> right)
        :  BinaryExpressionNode(left, right) {}
    ~BitwiseOrNode() = default;
    void accept(Visitor& v) override {v.visit(*this);}
};

class BitwiseXorNode : public BinaryExpressionNode {
public:
    BitwiseXorNode(std::shared_ptr<ExpressionNode> left, std::shared_ptr<ExpressionNode> right)
        :  BinaryExpressionNode(left, right) {}
    ~BitwiseXorNode() = default;
    void accept(Visitor& v) override {v.visit(*this);}
};

class BitwiseLeftShiftNode : public BinaryExpressionNode {
public:
    BitwiseLeftShiftNode(std::shared_ptr<ExpressionNode> left, std::shared_ptr<ExpressionNode> right)
        :  BinaryExpressionNode(left, right) {}
    ~BitwiseLeftShiftNode() = default;
    void accept(Visitor& v) override {v.visit(*this);}
};

class BitwiseRightShiftNode : public BinaryExpressionNode {
public:
    BitwiseRightShiftNode(std::shared_ptr<ExpressionNode> left, std::shared_ptr<ExpressionNode> right)
        :  BinaryExpressionNode(left, right) {}
    ~BitwiseRightShiftNode() = default;
    void accept(Visitor& v) override {v.visit(*this);}
};

// --- Structural Nodes ---
struct FunctionArgument {
    Type type;
    std::string name;
};

struct TypeNode : public ASTNode {
    Type type_;
    TypeNode(Type type) : type_(type) {}
    void accept(Visitor& v) override {v.visit(*this);}
};

struct FunctionArgumentsNode : public ASTNode {
    std::list<FunctionArgument> arguments_;
    FunctionArgumentsNode() = default;
    FunctionArgumentsNode(std::list<FunctionArgument>&& arguments) 
        : arguments_(std::move(arguments)) {}
    void accept(Visitor& v) override {v.visit(*this);}
};

struct ReturnStatementNode : public StatementNode {
    Type type_;
    std::shared_ptr<ExpressionNode> return_value_;
    ReturnStatementNode(Type return_type, std::shared_ptr<ExpressionNode> return_value)
        : type_(return_type), return_value_(std::move(return_value)) {}
    void accept(Visitor& v) override {v.visit(*this);}
};

struct StatementBlockNode : public ASTNode {
    std::vector<std::shared_ptr<StatementNode>> statements_;
    StatementBlockNode() = default;
    StatementBlockNode(std::vector<std::shared_ptr<StatementNode>> statements) 
        : statements_(std::move(statements)) {}
    void accept(Visitor& v) override {v.visit(*this);}
};

struct FunctionNode : public ASTNode {
    std::string name_;
    std::shared_ptr<TypeNode> type_node_;
    std::shared_ptr<FunctionArgumentsNode> arguments_node_;
    std::shared_ptr<StatementBlockNode> body_;

    FunctionNode(std::string name, std::shared_ptr<TypeNode> type_node, 
                 std::shared_ptr<FunctionArgumentsNode> arguments, 
                 std::shared_ptr<StatementBlockNode> body)
    : name_(std::move(name)), type_node_(std::move(type_node)), 
      arguments_node_(std::move(arguments)), body_(std::move(body)) {}
    void accept(Visitor& v) override {v.visit(*this);}
};

struct ProgramNode : public ASTNode {
    std::vector<std::shared_ptr<FunctionNode>> functions_;
    ProgramNode() = default;
    ProgramNode(std::vector<std::shared_ptr<FunctionNode>> functions) 
        : functions_(std::move(functions)) {}
    void accept(Visitor& v) override {v.visit(*this);}
};

} // namespace CAst
#endif // _AST_H_