#include "src/ast/x86_64_asm.h"
#include <fstream>
#include <string>
#include <vector>

namespace Codegen {

class ASMDumper : public ASM::Visitor {
public:
    ~ASMDumper();
    ASMDumper(std::string filename);
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
    void visit(ASM::DivNode& node) override;
    void visit(ASM::MultNode& node) override;
    void visit(ASM::AddNode& node) override;
    void visit(ASM::SubNode& node) override;
    void visit(ASM::CDQNode& node) override;
    void visit(ASM::BitwiseAndNode& node) override;
    void visit(ASM::BitwiseOrNode& node) override;
    void visit(ASM::BitwiseXorNode& node) override;
    void visit(ASM::SalNode& node) override;
    void visit(ASM::SarNode& node) override;
    void visit(ASM::CmpNode& node) override;
    void visit(ASM::SetCCNode& node) override;
    void visit(ASM::JumpNode& node) override;
    void visit(ASM::JumpCCNode& node) override;
    void visit(ASM::LabelNode& node) override;
    void dump_assembly(std::shared_ptr<ASM::ProgramNode> asm_program);
    std::vector<std::string> asm_text_dump_, buffer_;
    std::string target_filename_;
    std::ofstream of;
};

}