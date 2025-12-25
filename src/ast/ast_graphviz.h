#ifndef AST_GRAPHVIZ_H
#define AST_GRAPHVIZ_H

#include "src/ast/ast.h"
#include <fstream>

namespace CAst {

class GraphvizCAstVisitor : public Visitor {
public:
    ~GraphvizCAstVisitor();
    GraphvizCAstVisitor(std::string filename);
    void visit(TypeNode& node) override;
    void visit(FunctionArgumentsNode& node) override;
    void visit(ReturnStatementNode& node) override;
    void visit(StatementBlockNode& node) override;
    void visit(FunctionNode& node) override;
    void visit(ProgramNode& node) override;
    void visit(TildeUnaryExpressionNode& node) override;
    void visit(MinusUnaryExpressionNode& node) override;
    void visit(IntegerValueNode& node) override;
    std::vector<int> parents;
    int node_count_;
    std::ofstream of;
};

} // namespace CAst

#endif // AST_GRAPHVIZ_H