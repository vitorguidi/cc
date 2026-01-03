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
    void visit(CAst::DivNode& node) override;
    void visit(CAst::MultNode& node) override;
    void visit(CAst::ModNode& node) override;
    void visit(CAst::MinusNode& node) override;
    void visit(CAst::PlusNode& node) override;
    void visit(CAst::AndNode& node) override;
    void visit(CAst::OrNode& node) override;
    void visit(CAst::BitwiseAndNode& node) override;
    void visit(CAst::BitwiseOrNode& node) override;
    void visit(CAst::BitwiseXorNode& node) override;
    void visit(CAst::BitwiseRightShiftNode& node) override;
    void visit(CAst::BitwiseLeftShiftNode& node) override;
    void visit(CAst::NotUnaryExpressionNode& node) override;

    void visit_bin_exp(std::string node_name, CAst::BinaryExpressionNode& node);
    void visit_un_exp(std::string node_name, CAst::UnaryExpressionNode& node);
    std::string visit_child(std::string parent_id, std::string edge_name, std::shared_ptr<CAst::ASTNode> child_node);

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
    void visit(Tacky::NotNode& node) override;
    void visit(Tacky::IntegerNode& node) override;
    void visit(Tacky::VariableNode& node) override;
    void visit(Tacky::DivNode& node) override;
    void visit(Tacky::MultNode& node) override;
    void visit(Tacky::ModNode& node) override;
    void visit(Tacky::PlusNode& node) override;
    void visit(Tacky::MinusNode& node) override;
    void visit(Tacky::AndNode& node) override;
    void visit(Tacky::BitwiseAndNode& node) override;
    void visit(Tacky::OrNode& node) override;
    void visit(Tacky::BitwiseOrNode& node) override;
    void visit(Tacky::BitwiseLeftShiftNode& node) override;
    void visit(Tacky::BitwiseRightShiftNode& node) override;
    void visit(Tacky::BitwiseXorNode& node) override;

    void visit_bin_exp(std::string node_name, Tacky::BinaryOpNode& node);
    void visit_un_exp(std::string node_name, Tacky::UnaryNode& node);
    std::string visit_child(std::string parent_id, std::string edge_name, std::shared_ptr<Tacky::AstNode> child_node);

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
    std::vector<std::string> buffer_;
    std::ofstream of;
    int node_count_;
};

} // namespace Graphviz

#endif // AST_GRAPHVIZ_H