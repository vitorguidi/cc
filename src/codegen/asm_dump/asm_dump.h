#include "src/asm/asm_ast.h"
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
    void visit(ASM::NegNode& node) override;
    void visit(ASM::NotNode& node) override;
    void visit(ASM::MovNode& node) override;
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
    void dump_assembly(std::shared_ptr<ASM::ProgramNode> asm_program);
    std::vector<std::string> asm_text_dump_, buffer_;
    std::string target_filename_;
    std::ofstream of;
};

}