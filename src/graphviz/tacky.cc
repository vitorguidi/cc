#include "src/graphviz/graphviz.h"
#include <string>


namespace Graphviz {

GraphvizTackyVisitor::GraphvizTackyVisitor(std::string filename) : node_count_(0) {
    of = std::ofstream(filename);
    if (!of.is_open()) {
        throw std::runtime_error("unable to open file for graphviz output");
    }
    of << "Digraph CAst {\n";
    of << "\tnode [shape=record, fontname=\"Courier\"];\n";
}
GraphvizTackyVisitor::~GraphvizTackyVisitor() {
    if (of.is_open()) {
        of << "}\n";
        of.close();
    }
}

std::string GraphvizTackyVisitor::visit_child(std::string parent_id, std::string edge_name, std::shared_ptr<Tacky::AstNode> child_node) {
    child_node->accept(*this);
    auto child_id = buffer_.back();
    buffer_.pop_back();
    auto edge = labeled_edge(parent_id, child_id, edge_name);
    of << edge;
    return child_id;
}

void GraphvizTackyVisitor::visit_bin_exp(std::string node_name, Tacky::BinaryOpNode& node) {
    auto my_id = std::to_string(node_count_++);
    std::string node_repr = labeled_node_with_kv_pairs(
        my_id,
        node_name,
        {}
    );
    of << node_repr;

    visit_child(my_id, "left", node.left_);
    visit_child(my_id, "right", node.right_);
    visit_child(my_id, "dst", node.dst_);

    buffer_.push_back(my_id);
}

void GraphvizTackyVisitor::visit_un_exp(std::string node_name, Tacky::UnaryNode& node) {
    auto my_id = std::to_string(node_count_++);
    std::string node_repr = labeled_node_with_kv_pairs(
        my_id,
        node_name,
        {}
    );
    of << node_repr;
    visit_child(my_id, "src", node.src_);
    visit_child(my_id, "dst", node.dst_);
    buffer_.push_back(my_id);
}

void GraphvizTackyVisitor::visit(Tacky::ProgramNode& node) {
    auto my_id = std::to_string(node_count_++);
    std::string node_repr = labeled_node_with_kv_pairs(
        my_id,
        "ProgramNode",
        {}
    );
    of << node_repr;
    for (auto &fn : node.functions_) {
        visit_child(my_id, "function", fn);
    }
    buffer_.push_back(my_id);
}

void GraphvizTackyVisitor::visit(Tacky::FunctionNode& node) {
    auto my_id = std::to_string(node_count_++);
    std::string node_repr = labeled_node_with_kv_pairs(
        my_id,
        "FunctionNode",
        {std::make_pair("name", node.name_)}
    );
    of << node_repr;
    auto last_parent = my_id;
    for(auto& instruction : node.instructions_) {
        last_parent = visit_child(last_parent, "next instruction", instruction);
    }
    buffer_.push_back(my_id);
}

void GraphvizTackyVisitor::visit(Tacky::ReturnNode& node) {
    auto my_id = std::to_string(node_count_++);
    std::string node_repr = labeled_node_with_kv_pairs(
        my_id,
        "ReturnNode",
        {}
    );
    of << node_repr;
    visit_child(my_id, "return value", node.value_);
    buffer_.push_back(my_id);
}

// Unary expressions
void GraphvizTackyVisitor::visit(Tacky::ComplementNode& node) {visit_un_exp("ComplementNode", node);}
void GraphvizTackyVisitor::visit(Tacky::NegateNode& node) {visit_un_exp("NegateNode", node);}
void GraphvizTackyVisitor::visit(Tacky::NotNode& node) {visit_un_exp("NotNode", node);}

// Binary arithmetic expressions
void GraphvizTackyVisitor::visit(Tacky::DivNode& node) {visit_bin_exp("DivNode", node);}
void GraphvizTackyVisitor::visit(Tacky::MultNode& node) {visit_bin_exp("MultNode", node);}
void GraphvizTackyVisitor::visit(Tacky::ModNode& node) {visit_bin_exp("ModNode", node);}
void GraphvizTackyVisitor::visit(Tacky::PlusNode& node) {visit_bin_exp("PlusNode", node);}
void GraphvizTackyVisitor::visit(Tacky::MinusNode& node) {visit_bin_exp("MinusNode", node);}

// Binary boolean expressions
void GraphvizTackyVisitor::visit(Tacky::AndNode& node) {visit_bin_exp("AndNode", node);}
void GraphvizTackyVisitor::visit(Tacky::BitwiseAndNode& node) {visit_bin_exp("BitwiseAndNode", node);}
void GraphvizTackyVisitor::visit(Tacky::OrNode& node) {visit_bin_exp("OrNode", node);}
void GraphvizTackyVisitor::visit(Tacky::BitwiseOrNode& node) {visit_bin_exp("BitwiseOrNode", node);}
void GraphvizTackyVisitor::visit(Tacky::BitwiseLeftShiftNode& node) {visit_bin_exp("BitwiseLeftShiftNode", node);}
void GraphvizTackyVisitor::visit(Tacky::BitwiseRightShiftNode& node) {visit_bin_exp("BitwiseRightShiftNode", node);}
void GraphvizTackyVisitor::visit(Tacky::BitwiseXorNode& node) {visit_bin_exp("BitwiseXorNode", node);}

void GraphvizTackyVisitor::visit(Tacky::IntegerNode& node) {
    auto my_id = std::to_string(node_count_++);
    std::string node_repr = labeled_node_with_kv_pairs(
        my_id,
        "IntegerNode",
        {std::make_pair("value", std::to_string(node.value_))}
    );
    of << node_repr;
    buffer_.push_back(my_id);
}

void GraphvizTackyVisitor::visit(Tacky::VariableNode& node) {
    auto my_id = std::to_string(node_count_++);
    std::string node_repr = labeled_node_with_kv_pairs(
        my_id,
        "VariableNode",
        {std::make_pair("id", node.name_)}
    );
    of << node_repr;
    buffer_.push_back(my_id);
}

} //namespace Graphviz