#ifndef AST_GRAPHVIZ_H
#define AST_GRAPHVIZ_H

#include "src/ast/ast.h"
#include "src/tacky/tacky.h"
#include "src/asm/asm_ast.h"
#include <fstream>
#include <utility>
#include <string>

namespace Graphviz {

typedef std::pair<std::string, std::string> NodeKVPair;

std::string labeled_node_with_kv_pairs(std::string node_id, std::string node_name, const std::vector<NodeKVPair>& kv_pairs);
std::string labeled_edge(std::string parent_id, std::string child_id, std::string label);

class GraphvizCAstVisitor : public CAst::Visitor {
public:
    ~GraphvizCAstVisitor();
    GraphvizCAstVisitor(std::string filename);
    void visit(CAst::TypeNode& node) override;
    void visit(CAst::FunctionArgumentsNode& node) override;
    void visit(CAst::ReturnStatementNode& node) override;
    void visit(CAst::StatementBlockNode& node) override;
    void visit(CAst::FunctionNode& node) override;
    void visit(CAst::ProgramNode& node) override;
    void visit(CAst::TildeUnaryExpressionNode& node) override;
    void visit(CAst::MinusUnaryExpressionNode& node) override;
    void visit(CAst::IntegerValueNode& node) override;
    std::vector<std::string> buffer_;
    int node_count_;
    std::ofstream of;
};

class GraphvizTackyVisitor : public Tacky::Visitor {
public:
    ~GraphvizTackyVisitor();
    GraphvizTackyVisitor(std::string filename);
    void visit(Tacky::ProgramNode& node) override;
    void visit(Tacky::FunctionNode& node) override;
    void visit(Tacky::ReturnNode& node) override;
    void visit(Tacky::ComplementNode& node) override;
    void visit(Tacky::NegateNode& node) override;
    void visit(Tacky::IntegerNode& node) override;
    void visit(Tacky::VariableNode& node) override;
    std::vector<std::string> buffer_;
    std::ofstream of;
    int node_count_;
};

class GraphvizASMVisitor : public ASM::Visitor {
public:
    ~GraphvizASMVisitor();
    GraphvizASMVisitor(std::string filename);
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
    std::vector<std::string> buffer_;
    std::ofstream of;
    int node_count_;
};

} // namespace CAst

#endif // AST_GRAPHVIZ_H