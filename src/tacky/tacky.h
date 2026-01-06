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
class BitwiseNotNode;
class NotNode;
class ComplementNode;
class ValueNode;
class IntegerNode;
class VariableNode;
class BinaryOpNode;
class DivNode;
class ModNode;
class MultNode;
class PlusNode;
class MinusNode;
class BitwiseAndNode;
class BitwiseOrNode;
class BitwiseXorNode;
class BitwiseLeftShiftNode;
class BitwiseRightShiftNode;
class EqualNode;
class NotEqualNode;
class GreaterNode;
class GreaterEqNode;
class LessNode;
class LessEqNode;
class JumpNode;
class JumpIfZeroNode;
class JumpIfNotZeroNode;
class LabelNode;
class MovNode;
class NullNode;

class Visitor {
public:
    virtual ~Visitor() = default;
    virtual void visit(ProgramNode& node) = 0;
    virtual void visit(FunctionNode& node) = 0;
    virtual void visit(ReturnNode& node) = 0;
    virtual void visit(BitwiseNotNode& node) = 0;
    virtual void visit(ComplementNode& node) = 0;
    virtual void visit(IntegerNode& node) = 0;
    virtual void visit(VariableNode& node) = 0;
    virtual void visit(DivNode& node) = 0;
    virtual void visit(ModNode& node) = 0;
    virtual void visit(MultNode& node) = 0;
    virtual void visit(PlusNode& node) = 0;
    virtual void visit(MinusNode& node) = 0;
    virtual void visit(Tacky::NotNode& node) = 0;
    virtual void visit(Tacky::BitwiseAndNode& node) = 0;
    virtual void visit(Tacky::BitwiseOrNode& node) = 0;
    virtual void visit(Tacky::BitwiseLeftShiftNode& node) = 0;
    virtual void visit(Tacky::BitwiseRightShiftNode& node) = 0;
    virtual void visit(Tacky::BitwiseXorNode& node) = 0;
    virtual void visit(Tacky::EqualNode& node) = 0;
    virtual void visit(Tacky::NotEqualNode& node) = 0;
    virtual void visit(Tacky::GreaterNode& node) = 0;
    virtual void visit(Tacky::GreaterEqNode& node) = 0;
    virtual void visit(Tacky::LessNode& node) = 0;
    virtual void visit(Tacky::LessEqNode& node) = 0;
    virtual void visit(Tacky::JumpNode& node) = 0;
    virtual void visit(Tacky::JumpIfZeroNode& node) = 0;
    virtual void visit(Tacky::JumpIfNotZeroNode& node) = 0;
    virtual void visit(Tacky::MovNode& node) = 0;
    virtual void visit(Tacky::LabelNode& node) = 0;
    virtual void visit(Tacky::NullNode& node) = 0;
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

class BitwiseNotNode : public UnaryNode {
public:
    ~BitwiseNotNode() = default;
    BitwiseNotNode(std::shared_ptr<ValueNode> src, std::shared_ptr<ValueNode> dst) : UnaryNode(src, dst) {}
    void accept(Visitor& v) override { v.visit(*this); }
};

class ComplementNode : public UnaryNode {
public:
    ~ComplementNode() = default;
    ComplementNode(std::shared_ptr<ValueNode> src, std::shared_ptr<ValueNode> dst) : UnaryNode(src, dst) {}
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

class BitwiseAndNode : public BinaryOpNode {
public:
    ~BitwiseAndNode() = default;
    BitwiseAndNode(std::shared_ptr<ValueNode> left, std::shared_ptr<ValueNode> right, std::shared_ptr<ValueNode> dst)
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

class RelationalOpNode : public BinaryOpNode {
public:
    virtual ~RelationalOpNode() = default;
    RelationalOpNode(std::shared_ptr<ValueNode> left, std::shared_ptr<ValueNode> right, std::shared_ptr<ValueNode> dst)
        :   BinaryOpNode(left, right, dst) {}
    virtual void accept(Visitor& v) = 0;
};

class EqualNode : public RelationalOpNode {
public:
    ~EqualNode() = default;
    EqualNode(std::shared_ptr<ValueNode> left, std::shared_ptr<ValueNode> right, std::shared_ptr<ValueNode> dst)
        : RelationalOpNode(left, right, dst) {}
    void accept(Visitor& v) {v.visit(*this);}  
};

class NotEqualNode : public RelationalOpNode {
public:
    ~NotEqualNode() = default;
    NotEqualNode(std::shared_ptr<ValueNode> left, std::shared_ptr<ValueNode> right, std::shared_ptr<ValueNode> dst)
        : RelationalOpNode(left, right, dst) {}
    void accept(Visitor& v) {v.visit(*this);}  
};

class GreaterNode : public RelationalOpNode {
public:
    ~GreaterNode() = default;
    GreaterNode(std::shared_ptr<ValueNode> left, std::shared_ptr<ValueNode> right, std::shared_ptr<ValueNode> dst)
        : RelationalOpNode(left, right, dst) {}
    void accept(Visitor& v) {v.visit(*this);}  
};

class GreaterEqNode : public RelationalOpNode {
public:
    ~GreaterEqNode() = default;
    GreaterEqNode(std::shared_ptr<ValueNode> left, std::shared_ptr<ValueNode> right, std::shared_ptr<ValueNode> dst)
        : RelationalOpNode(left, right, dst) {}
    void accept(Visitor& v) {v.visit(*this);}  
};

class LessNode : public RelationalOpNode {
public:
    ~LessNode() = default;
    LessNode(std::shared_ptr<ValueNode> left, std::shared_ptr<ValueNode> right, std::shared_ptr<ValueNode> dst)
        : RelationalOpNode(left, right, dst) {}
    void accept(Visitor& v) {v.visit(*this);}  
};

class LessEqNode : public RelationalOpNode {
public:
    ~LessEqNode() = default;
    LessEqNode(std::shared_ptr<ValueNode> left, std::shared_ptr<ValueNode> right, std::shared_ptr<ValueNode> dst)
        : RelationalOpNode(left, right, dst) {}
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

class MovNode : public InstructionNode {
public:
    std::shared_ptr<ValueNode> src_, dst_;
    ~MovNode() = default;
    MovNode(std::shared_ptr<ValueNode> src, std::shared_ptr<ValueNode> dst)
        :   src_(src), dst_(dst) {}
    void accept(Visitor& v) {v.visit(*this);}
};

class LabelNode : public InstructionNode {
public:
    std::string name_;
    ~LabelNode() = default;
    LabelNode(std::string name) : name_(std::move(name)) {}
    void accept(Visitor& v) {v.visit(*this);}
};

class JumpNode : public InstructionNode {
public:
    std::shared_ptr<LabelNode> dst_;
    ~JumpNode() = default;
    JumpNode(std::shared_ptr<LabelNode> dst) : dst_(dst) {}
    void accept(Visitor& v) {v.visit(*this);}
};

class ConditionalJumpNode : public InstructionNode {
public:
    std::shared_ptr<LabelNode> dst_;
    std::shared_ptr<ValueNode> operand_;
    ~ConditionalJumpNode() = default;
    ConditionalJumpNode(std::shared_ptr<ValueNode> operand, std::shared_ptr<LabelNode> dst)
        : dst_(dst), operand_(operand) {}
    virtual void accept(Visitor& v) = 0; 
};

class JumpIfZeroNode : public ConditionalJumpNode {
public:
    ~JumpIfZeroNode() = default;
    JumpIfZeroNode(std::shared_ptr<ValueNode> operand, std::shared_ptr<LabelNode> dst)
        : ConditionalJumpNode(operand, dst) {}
    void accept(Visitor& v) {v.visit(*this);}
};

class JumpIfNotZeroNode : public ConditionalJumpNode {
public:
    ~JumpIfNotZeroNode() = default;
    JumpIfNotZeroNode(std::shared_ptr<ValueNode> operand, std::shared_ptr<LabelNode> dst)
        : ConditionalJumpNode(operand, dst) {}
    void accept(Visitor& v) override {v.visit(*this);}
};

class NullNode : public AstNode {
public:
    ~NullNode() = default;
    NullNode() = default;
    void accept(Visitor& v) override {v.visit(*this);}
};

} // namespace Tacky

#endif // TACKY_H