#ifndef ASM_H
#define ASM_H

#include <memory>
#include <vector>
#include <string>

namespace ASM {

class Visitor {
public:
    void visit(ProgramNode& node);
    void visit(FunctionNode& node);
    void visit(NegNode& node);
    void visit(NotNode& node);
    void visit(MovNode& node);
    void visit(RetNode& node);
    void visit(AllocateStackNode& node);
    void visit(ImmNode& node);
    void visit(StackNode& node);
    void visit(RegisterNode& node);
    void visit(PseudoNode& node);
};

class AstNode {
public:
    virtual ~AstNode() = 0;
    virtual void accept(Visitor& v) = 0;
    AstNode() = default;
    AstNode(AstNode&& that) = delete;
    AstNode operator=(AstNode&& that) = delete;
    AstNode(AstNode& that) = delete;
    AstNode operator=(AstNode& that) = delete;
};

class ProgramNode : public AstNode {
public:
    ~ProgramNode() = default;
    void accept(Visitor& v) override {v.visit(*this);}
    ProgramNode(std::vector<std::shared_ptr<FunctionNode>> functions)
        : functions_(std::move(functions)) {}
    std::vector<std::shared_ptr<FunctionNode>> functions_;
};

class FunctionNode : public AstNode {
public:
    ~FunctionNode() = default;
    void accept(Visitor& v) override;
    FunctionNode(std::string name, std::vector<std::shared_ptr<InstructionNode>> instructions)
        : name_(name), instructions_(std::move(instructions)) {}
    void accept(Visitor& v) override {v.visit(*this);}
    std::vector<std::shared_ptr<InstructionNode>> instructions_;
    std::string name_;
};

class InstructionNode : public AstNode {
public:
    virtual ~InstructionNode() = 0;
    virtual void accept(Visitor& v) = 0;
    InstructionNode() = default;
};

class OperandNode : public AstNode {
public:
    virtual ~OperandNode() = 0;
    virtual void accept(Visitor& v) = 0;
};

class ImmNode : public OperandNode {
public:
    ~ImmNode() = default;
    ImmNode(int val) : val_(val) {}
    void accept(Visitor& v) override {v.visit(*this);}
    int val_;
};

enum Register {
    AX,
    R10,
};

class RegisterNode : public OperandNode {
public:
    ~RegisterNode() = default;
    RegisterNode(Register reg) : reg_(reg) {}
    void accept(Visitor& v) override {v.visit(*this);}
    Register reg_;
};

class PseudoNode : public OperandNode {
public:
    ~PseudoNode() = default;
    PseudoNode(std::string name) : name_(name) {}
    void accept(Visitor& v) override {v.visit(*this);}
    std::string name_;
};

class StackNode : public OperandNode {
    ~StackNode() = default;
    StackNode(int size) : size_(size) {}
    void accept(Visitor& v) override {v.visit(*this);}
    int size_;
};

class UnaryInstructionNode : public InstructionNode {
public:
    virtual ~UnaryInstructionNode() = 0;
    UnaryInstructionNode(std::shared_ptr<OperandNode> src) : src_(src) {}
    std::shared_ptr<OperandNode> src_;
};

class NegNode : public UnaryInstructionNode {
public:
    ~NegNode() = default;
    NegNode(std::shared_ptr<OperandNode> src) : UnaryInstructionNode(src) {}
    void accept(Visitor& v) override {v.visit(*this);}
};

class NotNode : public UnaryInstructionNode {
public:
    ~NotNode() = default;
    NotNode(std::shared_ptr<OperandNode> src) : UnaryInstructionNode(src) {}
    void accept(Visitor& v) override {v.visit(*this);}
};

class MovNode : public InstructionNode {
public:
    ~MovNode() = default;
    MovNode(std::shared_ptr<OperandNode> src, std::shared_ptr<OperandNode> dst)
        : src_(src), dst_(dst) {}
    std::shared_ptr<OperandNode> src_, dst_;
};

class AllocateStackNode : public InstructionNode {
public:
    ~AllocateStackNode() = default;
    AllocateStackNode(int size) : size_(size) {};
    void accept(Visitor& v) override {v.visit(*this);}
    int size_;
};

class RetNode : public InstructionNode {
public:
    ~RetNode() = default;
    RetNode() = default;
    void accept(Visitor& v) override {v.visit(*this);}
};

} //namespace ASM
#endif // ASM_H