#ifndef _C_H_
#define _C_H_

#include <vector>
#include <list>
#include <string>
#include <memory>

namespace CAst {

enum Type {
    INTEGER,
    VOID,
};

inline std::string type_as_str(Type type) {
    switch (type) {
        case (Type::INTEGER) :
            return std::string("int");
        case Type::VOID :
            return std::string("void");
    }
    return std::string("");
};

// Forward declarations
class Visitor;
struct TypeNode;
struct AstNode;
struct ExpressionNode;
struct FunctionArgumentsNode;
struct ReturnStatementNode;
struct BlockNode;
struct FunctionNode;
struct ProgramNode;
struct IntegerValueNode;
struct BitwiseNotUnaryExpressionNode;
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
struct EqualNode;
struct NotEqualNode;
struct GreaterNode;
struct GreaterEqNode;
struct LessNode;
struct LessEqNode;
struct AssignmentNode;
struct DeclarationNode;
struct NullNode;
struct VariableNode;
struct IfNode;
struct TernaryNode;

// --- Visitor Interface ---
class Visitor {
public:
    virtual ~Visitor() = default;
    virtual void visit(TypeNode& node) = 0;
    virtual void visit(FunctionArgumentsNode& node) = 0;
    virtual void visit(ReturnStatementNode& node) = 0;
    virtual void visit(BlockNode& node) = 0;
    virtual void visit(FunctionNode& node) = 0;
    virtual void visit(BitwiseNotUnaryExpressionNode& node) = 0;
    virtual void visit(MinusUnaryExpressionNode& node) = 0;
    virtual void visit(NotUnaryExpressionNode& node) = 0;
    virtual void visit(AndNode& node) = 0;
    virtual void visit(OrNode& node) = 0;
    virtual void visit(BitwiseXorNode& node) = 0;
    virtual void visit(BitwiseAndNode& node) = 0;
    virtual void visit(BitwiseOrNode& node) = 0;
    virtual void visit(BitwiseLeftShiftNode& node) = 0;
    virtual void visit(BitwiseRightShiftNode& node) = 0;
    virtual void visit(EqualNode& node) = 0;
    virtual void visit(NotEqualNode& node) = 0;
    virtual void visit(GreaterNode& node) = 0;
    virtual void visit(GreaterEqNode& node) = 0;
    virtual void visit(LessNode& node) = 0;
    virtual void visit(LessEqNode& node) = 0;
    virtual void visit(IntegerValueNode& node) = 0;
    virtual void visit(ModNode& node) = 0;
    virtual void visit(DivNode& node) = 0;
    virtual void visit(MultNode& node) = 0;
    virtual void visit(PlusNode& node) = 0;
    virtual void visit(MinusNode& node) = 0;
    virtual void visit(ProgramNode& node) = 0;
    virtual void visit(DeclarationNode& node) = 0;
    virtual void visit(AssignmentNode& node) = 0;
    virtual void visit(NullNode& node) = 0;
    virtual void visit(VariableNode& node) = 0;
    virtual void visit(IfNode& node) = 0;
    virtual void visit(TernaryNode& node) = 0;
};

// --- Base Nodes ---
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void accept(Visitor& v) = 0;
};

class BlockElementNode : public ASTNode {
public:
    virtual ~BlockElementNode() = default;
    virtual void accept(Visitor& v) = 0;
};


class StatementNode : public BlockElementNode {
public:
    virtual ~StatementNode() = default;
    virtual void accept(Visitor& v) override = 0;
};

class ExpressionNode : public StatementNode {
public:
    virtual ~ExpressionNode() = default;
    virtual void accept(Visitor& v) override = 0;
};

class DeclarationNode : public BlockElementNode {
public:
    std::shared_ptr<VariableNode> var_;
    std::shared_ptr<TypeNode> type_;
    std::optional<std::shared_ptr<ExpressionNode>> expr_;
    ~DeclarationNode() = default;
    DeclarationNode(
        std::shared_ptr<VariableNode> var,
        std::shared_ptr<TypeNode> type,
        std::optional<std::shared_ptr<ExpressionNode>> expr)
        : var_(var), type_(type), expr_(expr) {}
    void accept(Visitor& v) override {v.visit(*this);}
};

class NullNode : public StatementNode {
public:
    ~NullNode() = default;
    NullNode() = default;
    void accept(Visitor& v) override {v.visit(*this);};
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

class VariableNode : public ExpressionNode {
public:
    std::string name_;
    VariableNode() = default;
    VariableNode(std::string name) : name_(std::move(name)) {}
    void accept(Visitor& v) override {v.visit(*this);}
};

class UnaryExpressionNode : public ExpressionNode {
public:
    std::shared_ptr<ExpressionNode> operand_;
    virtual ~UnaryExpressionNode() = default;
    UnaryExpressionNode(std::shared_ptr<ExpressionNode> operand)
        : operand_(operand) {}

};

class BitwiseNotUnaryExpressionNode : public UnaryExpressionNode {
public:
    BitwiseNotUnaryExpressionNode(std::shared_ptr<ExpressionNode> operand) 
        : UnaryExpressionNode(operand) {}
    void accept(Visitor& v) override {v.visit(*this);}
};

class MinusUnaryExpressionNode : public UnaryExpressionNode {
public:
    MinusUnaryExpressionNode(std::shared_ptr<ExpressionNode> operand) 
        : UnaryExpressionNode(operand) {}
    void accept(Visitor& v) override {v.visit(*this);}
};

class NotUnaryExpressionNode : public UnaryExpressionNode {
public:
    NotUnaryExpressionNode(std::shared_ptr<ExpressionNode> operand)
        : UnaryExpressionNode(operand) {}
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

class EqualNode : public BinaryExpressionNode {
public:
    EqualNode(std::shared_ptr<ExpressionNode> left, std::shared_ptr<ExpressionNode> right)
        :  BinaryExpressionNode(left, right) {}
    ~EqualNode() = default;
    void accept(Visitor& v) override {v.visit(*this);}
};

class NotEqualNode : public BinaryExpressionNode {
public:
    NotEqualNode(std::shared_ptr<ExpressionNode> left, std::shared_ptr<ExpressionNode> right)
        :  BinaryExpressionNode(left, right) {}
    ~NotEqualNode() = default;
    void accept(Visitor& v) override {v.visit(*this);}
};

class GreaterNode : public BinaryExpressionNode {
public:
    GreaterNode(std::shared_ptr<ExpressionNode> left, std::shared_ptr<ExpressionNode> right)
        :  BinaryExpressionNode(left, right) {}
    ~GreaterNode() = default;
    void accept(Visitor& v) override {v.visit(*this);}
};

class GreaterEqNode : public BinaryExpressionNode {
public:
    GreaterEqNode(std::shared_ptr<ExpressionNode> left, std::shared_ptr<ExpressionNode> right)
        :  BinaryExpressionNode(left, right) {}
    ~GreaterEqNode() = default;
    void accept(Visitor& v) override {v.visit(*this);}
};

class LessNode : public BinaryExpressionNode {
public:
    LessNode(std::shared_ptr<ExpressionNode> left, std::shared_ptr<ExpressionNode> right)
        :  BinaryExpressionNode(left, right) {}
    ~LessNode() = default;
    void accept(Visitor& v) override {v.visit(*this);}
};

class LessEqNode : public BinaryExpressionNode {
public:
    LessEqNode(std::shared_ptr<ExpressionNode> left, std::shared_ptr<ExpressionNode> right)
        :  BinaryExpressionNode(left, right) {}
    ~LessEqNode() = default;
    void accept(Visitor& v) override {v.visit(*this);}
};

class AssignmentNode : public ExpressionNode {
public:
    std::shared_ptr<ExpressionNode> left_, right_;
    AssignmentNode(std::shared_ptr<ExpressionNode> left, std::shared_ptr<ExpressionNode> right)
        :   left_(left), right_(right) {}
    void accept(Visitor& v) override {v.visit(*this);}
};

class TernaryNode : public ExpressionNode {
public:
    std::shared_ptr<CAst::ExpressionNode> cond_, then_, alt_;
    ~TernaryNode() = default;
    TernaryNode(
        std::shared_ptr<CAst::ExpressionNode> cond,
        std::shared_ptr<CAst::ExpressionNode> then,
        std::shared_ptr<CAst::ExpressionNode> alt)
        :   cond_(cond), then_(then), alt_(alt) {}
    void accept(Visitor& v) override{v.visit(*this);}
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

struct IfNode : public StatementNode {
public:
    std::shared_ptr<ExpressionNode> cond_;
    std::shared_ptr<StatementNode> then_;
    std::optional<std::shared_ptr<StatementNode>> alt_;
    ~IfNode() = default;
    IfNode(std::shared_ptr<ExpressionNode> cond, std::shared_ptr<StatementNode> then)
        : cond_(cond), then_(then), alt_(std::nullopt) {}
    IfNode(
        std::shared_ptr<ExpressionNode> cond,
        std::shared_ptr<StatementNode> then,
        std::shared_ptr<StatementNode> alt) : cond_(cond), then_(then), alt_(std::make_optional(alt)) {}
    void accept(Visitor& v) override {v.visit(*this);}
};

struct BlockNode : public ASTNode {
    std::vector<std::shared_ptr<BlockElementNode>> statements_;
    BlockNode() = default;
    BlockNode(std::vector<std::shared_ptr<BlockElementNode>> statements) 
        : statements_(std::move(statements)) {}
    void accept(Visitor& v) override {v.visit(*this);}
};

struct FunctionNode : public ASTNode {
    std::string name_;
    std::shared_ptr<TypeNode> type_node_;
    std::shared_ptr<FunctionArgumentsNode> arguments_node_;
    std::shared_ptr<BlockNode> body_;

    FunctionNode(std::string name, std::shared_ptr<TypeNode> type_node, 
                 std::shared_ptr<FunctionArgumentsNode> arguments, 
                 std::shared_ptr<BlockNode> body)
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
#endif // _C_H_