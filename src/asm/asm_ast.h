#ifndef ASM_H
#define ASM_H

#include <memory>
#include <vector>
#include <string>

namespace ASM {

class AstNode;
class ProgramNode;
class FunctionNode;
class InstructionNode;
class NegNode;
class NotNode;
class MovNode;
class RetNode;
class AllocateStackNode;
class ImmNode;
class StackNode;
class RegisterNode;
class PseudoNode;

class Visitor {
public:
    virtual ~Visitor() = default;
    virtual void visit(ProgramNode& node) = 0;
    virtual void visit(FunctionNode& node) = 0;
    virtual void visit(NegNode& node) = 0;
    virtual void visit(NotNode& node) = 0;
    virtual void visit(MovNode& node) = 0;
    virtual void visit(RetNode& node) = 0;
    virtual void visit(AllocateStackNode& node) = 0;
    virtual void visit(ImmNode& node) = 0;
    virtual void visit(StackNode& node) = 0;
    virtual void visit(RegisterNode& node) = 0;
    virtual void visit(PseudoNode& node) = 0;
};

class AstNode {
public:
    virtual ~AstNode() = default;
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
    FunctionNode(std::string name, std::vector<std::shared_ptr<InstructionNode>> instructions)
        : name_(name), instructions_(std::move(instructions)) {}
    void accept(Visitor& v) override {v.visit(*this);}
    std::string name_;
    std::vector<std::shared_ptr<InstructionNode>> instructions_;
};

class InstructionNode : public AstNode {
public:
    virtual ~InstructionNode() = default;
    virtual void accept(Visitor& v) = 0;
    InstructionNode() = default;
};

class OperandNode : public AstNode {
public:
    virtual ~OperandNode() = default;
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

inline std::string register_as_string(Register reg) {
    switch(reg) {
        case Register::AX:
            return "AX";
        case Register::R10:
            return "R10";
        default:
            throw std::runtime_error("Unexpected register type for string conversion");
    }
}

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
public:
    ~StackNode() = default;
    StackNode(int size) : size_(size) {}
    void accept(Visitor& v) override {v.visit(*this);}
    int size_;
};

class UnaryInstructionNode : public InstructionNode {
public:
    virtual ~UnaryInstructionNode() = default;
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
    void accept(Visitor& v) override {v.visit(*this);}
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