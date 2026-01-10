#ifndef TACKY_TO_ASM_VISITOR_H
#define TACKY_TO_ASM_VISITOR_H

#include "src/ast/tacky.h"
#include "src/ast/x86_64_asm.h"
#include <stdexcept>
#include <unordered_map>
#include <deque>

namespace Backend {

template<typename T>
inline std::shared_ptr<T> As(std::shared_ptr<ASM::AstNode>& node, const std::string& error_msg) {
    auto casted_ptr = std::dynamic_pointer_cast<T>(node);
    if (!casted_ptr) {
        throw std::runtime_error(error_msg);
    }
    return casted_ptr;
}

class TackyToAsmVisitor : public Tacky::Visitor {
public:
    ~TackyToAsmVisitor() = default;
    TackyToAsmVisitor() = default;
    void visit(Tacky::ProgramNode& node) override;
    void visit(Tacky::FunctionNode& node) override;
    void visit(Tacky::FunctionCallNode& node) override;
    void visit(Tacky::ReturnNode& node) override;
    void visit(Tacky::BitwiseNotNode& node) override;
    void visit(Tacky::ComplementNode& node) override;
    void visit(Tacky::NotNode& node) override;
    void visit(Tacky::IntegerNode& node) override;
    void visit(Tacky::VariableNode& node) override;
    void visit(Tacky::DivNode& node) override;
    void visit(Tacky::ModNode& node) override;
    void visit(Tacky::MultNode& node) override;
    void visit(Tacky::PlusNode& node) override;
    void visit(Tacky::MinusNode& node) override;
    void visit(Tacky::BitwiseAndNode& node) override;
    void visit(Tacky::BitwiseOrNode& node) override;
    void visit(Tacky::BitwiseXorNode& node) override;
    void visit(Tacky::BitwiseLeftShiftNode& node) override;
    void visit(Tacky::BitwiseRightShiftNode& node) override;
    void visit(Tacky::EqualNode& node) override;
    void visit(Tacky::NotEqualNode& node) override;
    void visit(Tacky::GreaterNode& node) override;
    void visit(Tacky::GreaterEqNode& node) override;
    void visit(Tacky::LessNode& node) override;
    void visit(Tacky::LessEqNode& node) override;
    void visit(Tacky::MovNode& node) override;
    void visit(Tacky::LabelNode& node) override;
    void visit(Tacky::JumpNode& node) override;
    void visit(Tacky::JumpIfZeroNode& node) override;
    void visit(Tacky::JumpIfNotZeroNode& node) override;
    void visit(Tacky::NullNode& node) override;

    template<std::derived_from<ASM::BinInstructionNode> T>
    void visit_binexp(Tacky::BinaryOpNode& node);

    template<std::derived_from<ASM::UnaryInstructionNode> T>
    void visit_unexp(Tacky::UnaryNode& node);

    void visit_relational_exp(ASM::ConditionCode cc, Tacky::RelationalOpNode& node);
    void visit_conditional_jump(ASM::ConditionCode cc, Tacky::ConditionalJumpNode& node);

    std::shared_ptr<ASM::ProgramNode> get_asm_from_tacky(std::shared_ptr<Tacky::ProgramNode> tacky_program);
    std::vector<std::shared_ptr<ASM::AstNode>> result_buffer_, instruction_buffer_;
};

class x86_64_ASM_RewriteVisitor : public ASM::Visitor {
private:
    template <std::derived_from<ASM::BinInstructionNode> T>
    void visit_binexp(ASM::BinInstructionNode& node);
    template <std::derived_from<ASM::UnaryInstructionNode> T>
    void visit_unexp(ASM::UnaryInstructionNode& node);
public:
    ~x86_64_ASM_RewriteVisitor() = default;
    x86_64_ASM_RewriteVisitor() = default;
    void visit(ASM::ProgramNode& node) override;
    void visit(ASM::FunctionNode& node) override;
    void visit(ASM::ComplementNode& node) override;
    void visit(ASM::BitwiseNotNode& node) override;
    void visit(ASM::MovNode& node) override;
    void visit(ASM::MovBNode& node) override;
    void visit(ASM::RetNode& node) override;
    void visit(ASM::AllocateStackNode& node) override;
    void visit(ASM::ImmNode& node) override;
    void visit(ASM::StackNode& node) override;
    void visit(ASM::RegisterNode& node) override;
    void visit(ASM::PseudoNode& node) override;
    void visit(ASM::MultNode& node) override;
    void visit(ASM::DivNode& node) override;
    void visit(ASM::AddNode& node) override;
    void visit(ASM::SubNode& node) override;
    void visit(ASM::BitwiseAndNode& node) override;
    void visit(ASM::BitwiseOrNode& node) override;
    void visit(ASM::BitwiseXorNode& node) override;
    void visit(ASM::SalNode& node) override;
    void visit(ASM::SarNode& node) override;
    void visit(ASM::CDQNode& node) override;
    void visit(ASM::CmpNode& node) override;
    void visit(ASM::SetCCNode& node) override;
    void visit(ASM::LabelNode& node) override;
    void visit(ASM::JumpCCNode& node) override;
    void visit(ASM::JumpNode& node) override;
    void visit(ASM::CallNode& node) override;
    void visit(ASM::PushNode& node) override;
    void visit(ASM::NullNode& node) override;
    void visit(ASM::DeallocateStackNode& node) override;
    std::shared_ptr<ASM::ProgramNode> get_rewritten_asm_program(std::shared_ptr<ASM::ProgramNode> program);
    std::deque<std::shared_ptr<ASM::AstNode>> buffer_;
};

class PseudoReplacerVisitor : public x86_64_ASM_RewriteVisitor {
public:
    ~PseudoReplacerVisitor() = default;
    PseudoReplacerVisitor() : current_offset_(0) {}
    void visit(ASM::PseudoNode& node) override;
    void visit(ASM::FunctionNode& node) override;
    int get_offset();
    int current_offset_;
    std::unordered_map<std::string, int> stack_offsets_;
};

class InstructionFixUpVisitor : public x86_64_ASM_RewriteVisitor {
public:
    ~InstructionFixUpVisitor() = default;
    InstructionFixUpVisitor()
        : x86_64_ASM_RewriteVisitor() {}
    void visit(ASM::FunctionNode& node) override;
    void visit(ASM::MovNode& node) override;
    void visit(ASM::MovBNode& node) override;
    void visit(ASM::DivNode& node) override;
    void visit(ASM::MultNode& node) override;
    void visit(ASM::AddNode& node) override;
    void visit(ASM::SubNode& node) override;
    void visit(ASM::BitwiseAndNode& node) override;
    void visit(ASM::BitwiseOrNode& node) override;
    void visit(ASM::BitwiseXorNode& node) override;
    void visit(ASM::SalNode& node) override;
    void visit(ASM::SarNode& node) override;
    void visit(ASM::CmpNode& node) override;
};

} // namespace Backend

#endif // TACKY_TO_ASM_VISITOR_H