#ifndef TACKY_H
#define TACKY_H

#include "src/ast/ast.h"
#include <string>
#include <vector>
#include <memory>

namespace Tacky {

class Visitor;
class AstNode;
class ProgramNode;
class FunctionNode;
class InstructionNode;
class ReturnNode;
class UnaryNode;
class ComplementNode;
class NotNode;
class NegateNode;
class ValueNode;
class IntegerNode;
class VariableNode;
class BinaryOpNode;
class DivNode;
class ModNode;
class MultNode;
class PlusNode;
class MinusNode;
class AndNode;
class BitwiseAndNode;
class OrNode;
class BitwiseOrNode;
class BitwiseXorNode;
class BitwiseLeftShiftNode;
class BitwiseRightShiftNode;

class Visitor {
public:
    virtual ~Visitor() = default;
    virtual void visit(ProgramNode& node) = 0;
    virtual void visit(FunctionNode& node) = 0;
    virtual void visit(ReturnNode& node) = 0;
    virtual void visit(ComplementNode& node) = 0;
    virtual void visit(NegateNode& node) = 0;
    virtual void visit(IntegerNode& node) = 0;
    virtual void visit(VariableNode& node) = 0;
    virtual void visit(DivNode& node) = 0;
    virtual void visit(ModNode& node) = 0;
    virtual void visit(MultNode& node) = 0;
    virtual void visit(PlusNode& node) = 0;
    virtual void visit(MinusNode& node) = 0;
    virtual void visit(Tacky::NotNode& node) = 0;
    virtual void visit(Tacky::AndNode& node) = 0;
    virtual void visit(Tacky::BitwiseAndNode& node) = 0;
    virtual void visit(Tacky::OrNode& node) = 0;
    virtual void visit(Tacky::BitwiseOrNode& node) = 0;
    virtual void visit(Tacky::BitwiseLeftShiftNode& node) = 0;
    virtual void visit(Tacky::BitwiseRightShiftNode& node) = 0;
    virtual void visit(Tacky::BitwiseXorNode& node) = 0;
};


class AstNode {
public:
    virtual ~AstNode() = default;
    AstNode() = default;
    AstNode(AstNode&& that) = delete;
    AstNode operator=(AstNode&& that) = delete;
    AstNode(AstNode& that) = delete;
    AstNode operator=(AstNode& that) = delete;   
    virtual void accept(Visitor& v) = 0;
};

class ProgramNode : public AstNode {
public:
    ProgramNode(std::vector<std::shared_ptr<FunctionNode>> functions) : functions_(std::move(functions)) {}
    ~ProgramNode() = default;
    void accept(Visitor& v) override { v.visit(*this); }
    std::vector<std::shared_ptr<FunctionNode>> functions_;
};

class FunctionNode : public AstNode {
public:
    FunctionNode(std::string name, std::vector<std::shared_ptr<InstructionNode>> instructions) : 
        name_(std::move(name)), instructions_(std::move(instructions)) {}
    ~FunctionNode() = default;
    void accept(Visitor& v) override { v.visit(*this); }
    std::string name_;
    std::vector<std::shared_ptr<InstructionNode>> instructions_;
};

class InstructionNode : public AstNode {
public:
    virtual ~InstructionNode() = default;
    virtual void accept(Visitor& v) = 0;
};

class ReturnNode : public InstructionNode {
public:
    ~ReturnNode() = default;
    ReturnNode(std::shared_ptr<ValueNode> value) : value_(std::move(value)) {}
    void accept(Visitor& v) override { v.visit(*this); }
    std::shared_ptr<ValueNode> value_;
};

class UnaryNode : public InstructionNode {
public:
    virtual ~UnaryNode() = default;
    UnaryNode(std::shared_ptr<ValueNode> src, std::shared_ptr<ValueNode> dst)
        :src_(std::move(src)), dst_(std::move(dst)) {}
    virtual void accept(Visitor& v) = 0;
    std::shared_ptr<ValueNode> src_, dst_;
};

class ComplementNode : public UnaryNode {
public:
    ~ComplementNode() = default;
    ComplementNode(std::shared_ptr<ValueNode> src, std::shared_ptr<ValueNode> dst) : UnaryNode(src, dst) {}
    void accept(Visitor& v) override { v.visit(*this); }
};

class NegateNode : public UnaryNode {
public:
    ~NegateNode() = default;
    NegateNode(std::shared_ptr<ValueNode> src, std::shared_ptr<ValueNode> dst) : UnaryNode(src, dst) {}
    void accept(Visitor& v) override { v.visit(*this); }
};

class NotNode : public UnaryNode {
public:
    ~NotNode() = default;
    NotNode(std::shared_ptr<ValueNode> src, std::shared_ptr<ValueNode> dst) : UnaryNode(src, dst) {}
    void accept(Visitor& v) override {v.visit(*this);}  
};

class BinaryOpNode : public InstructionNode {
public:
    std::shared_ptr<ValueNode> left_, right_, dst_;
    ~BinaryOpNode() = default;
    BinaryOpNode(std::shared_ptr<ValueNode> left, std::shared_ptr<ValueNode> right, std::shared_ptr<ValueNode> dst)
        : left_(left), right_(right), dst_(dst) {}
    virtual void accept(Visitor& v) = 0;
};

class DivNode : public BinaryOpNode {
public:
    ~DivNode() = default;
    DivNode(std::shared_ptr<ValueNode> left, std::shared_ptr<ValueNode> right, std::shared_ptr<ValueNode> dst)
        : BinaryOpNode(left, right, dst) {}
    void accept(Visitor& v) {v.visit(*this);}
};

class ModNode : public BinaryOpNode {
public:
    ~ModNode() = default;
    ModNode(std::shared_ptr<ValueNode> left, std::shared_ptr<ValueNode> right, std::shared_ptr<ValueNode> dst)
        : BinaryOpNode(left, right, dst) {}
    void accept(Visitor& v) {v.visit(*this);}
};

class MultNode : public BinaryOpNode {
public:
    ~MultNode() = default;
    MultNode(std::shared_ptr<ValueNode> left, std::shared_ptr<ValueNode> right, std::shared_ptr<ValueNode> dst)
        : BinaryOpNode(left, right, dst) {}
    void accept(Visitor& v) {v.visit(*this);}
};

class MinusNode : public BinaryOpNode {
public:
    ~MinusNode() = default;
    MinusNode(std::shared_ptr<ValueNode> left, std::shared_ptr<ValueNode> right, std::shared_ptr<ValueNode> dst)
        : BinaryOpNode(left, right, dst) {}
    void accept(Visitor& v) {v.visit(*this);}
};

class PlusNode : public BinaryOpNode {
public:
    ~PlusNode() = default;
    PlusNode(std::shared_ptr<ValueNode> left, std::shared_ptr<ValueNode> right, std::shared_ptr<ValueNode> dst)
        : BinaryOpNode(left, right, dst) {}
    void accept(Visitor& v) {v.visit(*this);}
};

class AndNode : public BinaryOpNode {
public:
    ~AndNode() = default;
    AndNode(std::shared_ptr<ValueNode> left, std::shared_ptr<ValueNode> right, std::shared_ptr<ValueNode> dst)
        : BinaryOpNode(left, right, dst) {}
    void accept(Visitor& v) {v.visit(*this);}
};

class BitwiseAndNode : public BinaryOpNode {
public:
    ~BitwiseAndNode() = default;
    BitwiseAndNode(std::shared_ptr<ValueNode> left, std::shared_ptr<ValueNode> right, std::shared_ptr<ValueNode> dst)
        : BinaryOpNode(left, right, dst) {}
    void accept(Visitor& v) {v.visit(*this);}
};

class OrNode : public BinaryOpNode {
public:
    ~OrNode() = default;
    OrNode(std::shared_ptr<ValueNode> left, std::shared_ptr<ValueNode> right, std::shared_ptr<ValueNode> dst)
        : BinaryOpNode(left, right, dst) {}
    void accept(Visitor& v) {v.visit(*this);}
};

class BitwiseOrNode : public BinaryOpNode {
public:
    ~BitwiseOrNode() = default;
    BitwiseOrNode(std::shared_ptr<ValueNode> left, std::shared_ptr<ValueNode> right, std::shared_ptr<ValueNode> dst)
        : BinaryOpNode(left, right, dst) {}
    void accept(Visitor& v) {v.visit(*this);}
};

class BitwiseXorNode : public BinaryOpNode {
public:
    ~BitwiseXorNode() = default;
    BitwiseXorNode(std::shared_ptr<ValueNode> left, std::shared_ptr<ValueNode> right, std::shared_ptr<ValueNode> dst)
        : BinaryOpNode(left, right, dst) {}
    void accept(Visitor& v) {v.visit(*this);}
};

class BitwiseLeftShiftNode : public BinaryOpNode {
public:
    ~BitwiseLeftShiftNode() = default;
    BitwiseLeftShiftNode(std::shared_ptr<ValueNode> left, std::shared_ptr<ValueNode> right, std::shared_ptr<ValueNode> dst)
        : BinaryOpNode(left, right, dst) {}
    void accept(Visitor& v) {v.visit(*this);}
};

class BitwiseRightShiftNode : public BinaryOpNode {
public:
    ~BitwiseRightShiftNode() = default;
    BitwiseRightShiftNode(std::shared_ptr<ValueNode> left, std::shared_ptr<ValueNode> right, std::shared_ptr<ValueNode> dst)
        : BinaryOpNode(left, right, dst) {}
    void accept(Visitor& v) {v.visit(*this);}
};

class ValueNode : public AstNode {
public:
    virtual ~ValueNode() = default;
    virtual void accept(Visitor& v) = 0;
};

class IntegerNode : public ValueNode {
public:
    ~IntegerNode() = default;
    IntegerNode(int value) : value_(value) {}
    void accept(Visitor& v) override { v.visit(*this); }
    int value_;
};

class VariableNode : public ValueNode {
public:
    ~VariableNode() = default;
    VariableNode(std::string name) : name_(std::move(name)) {}
    void accept(Visitor& v) override { v.visit(*this); }
    std::string name_;
};

} // namespace Tacky

#endif // TACKY_H