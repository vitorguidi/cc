#include "src/ast/ast_graphviz.h"
#include <stdexcept>

namespace CAst {

GraphvizCAstVisitor::GraphvizCAstVisitor(std::string filename) : node_count_(0) {
    of = std::ofstream("output.dot");
    if (!of.is_open()) {
        throw std::runtime_error("unable to open file for graphviz output");
    }
    of << "Digraph CAst {\n";
    of << "\tnode [shape=record, fontname=\"Courier\"];\n";
}
GraphvizCAstVisitor::~GraphvizCAstVisitor() {
    if (of.is_open()) {
        of << "}\n";
        of.close();
    }
}

void GraphvizCAstVisitor::visit(ProgramNode& node) {
    of << "\tn" << node_count_ << " [label=\"Program\"];\n";
    parents.push_back(node_count_++);
    for(auto& function: node.functions_) {
        function->accept(*this);
    }
    parents.pop_back();
}

void GraphvizCAstVisitor::visit(FunctionNode& node) {
    of << "\tn" << node_count_ << " [label=\"Function : " << node.name_ << "\"];\n";
    if (!parents.empty()) {
        of << "\tn" << parents.back() << " -> n" << node_count_ << ";\n";
    }
    parents.push_back(node_count_++);
    node.type_node_->accept(*this);
    node.arguments_node_->accept(*this);
    node.body_->accept(*this);
    parents.pop_back();
}

void GraphvizCAstVisitor::visit(TypeNode& node) {
    std::string type = type_as_str(node.type_);
    of << "\tn" << node_count_ << " [label=\"Type: " << type <<  "\"];\n";
    if (!parents.empty()) {
        of << "\tn" << parents.back() << " -> n" << node_count_ << ";\n";
    }
    node_count_++;
}

void GraphvizCAstVisitor::visit(FunctionArgumentsNode& node) {
    of << "\tn" << node_count_ << " [label=\"{";
    of << "FunctionArgs }\"];\n";
    if (!parents.empty()) {
        of << "\tn" << parents.back() << " -> n" << node_count_ << ";\n";
    }
    for(auto x : node.arguments_) {
        std::string type_name = type_as_str(x.type);
        of << " | { " << type_name <<  " | " << x.name << " } ";
    }

    node_count_++;
}

void GraphvizCAstVisitor::visit(StatementBlockNode& node) {
    of << "\tn" << node_count_ << " [label=\"Statement Block\"];\n";
    if (!parents.empty()) {
        of << "\tn" << parents.back() << " -> n" << node_count_ << ";\n";
    }
    parents.push_back(node_count_++);
    for(auto& stmt : node.statements_) {
        stmt->accept(*this);
    }
    parents.pop_back();
}


void GraphvizCAstVisitor::visit(ReturnStatementNode& node) {
    of << "\tn" << node_count_ << " [label=\" ";
    of << "Return Statement\"];\n";
    if (!parents.empty()) {
        of << "\tn" << parents.back() << " -> n" << node_count_ << ";\n";
    }
    parents.push_back(node_count_++);
    node.return_value_->accept(*this);
    parents.pop_back();
}

void GraphvizCAstVisitor::visit(TildeUnaryExpressionNode& node) {
    of << "\tn" << node_count_ << " [label=\" ";
    of << "Negation Expression\"]\n";
    if (!parents.empty()) {
        of << "\tn" << parents.back() << " -> n" << node_count_ << ";\n";
    }
    parents.push_back(node_count_++);
    node.operand_->accept(*this);
    parents.pop_back();
}

void GraphvizCAstVisitor::visit(MinusUnaryExpressionNode& node) {
    of << "\tn" << node_count_ << " [label=\" ";
    of << "Complement Expression\"]\n";
    if (!parents.empty()) {
        of << "\tn" << parents.back() << " -> n" << node_count_ << ";\n";
    }
    parents.push_back(node_count_++);
    node.operand_->accept(*this);
    parents.pop_back();
}

void GraphvizCAstVisitor::visit(IntegerValueNode& node) {
    of << "\tn" << node_count_ << " [label=\" ";
    of << "Integer Value: " << node.value_ <<  "\"];\n";
    if (!parents.empty()) {
        of << "\tn" << parents.back() << " -> n" << node_count_ << ";\n";
    }
}

} // namespace CAst