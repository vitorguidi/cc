#include "src/graphviz/graphviz.h"

namespace Graphviz {

GraphvizASMVisitor::GraphvizASMVisitor(std::string filename) : node_count_(0) {
    of = std::ofstream(filename);
    if (!of.is_open()) {
        throw std::runtime_error("unable to open file for graphviz output");
    }
    of << "Digraph CAst {\n";
    of << "\tnode [shape=record, fontname=\"Courier\"];\n";
}

GraphvizASMVisitor::~GraphvizASMVisitor() {
    if (of.is_open()) {
        of << "}\n";
        of.close();
    }
}

std::string GraphvizASMVisitor::visit_child(std::string parent_id, std::string edge_name, std::shared_ptr<ASM::AstNode> child_node) {
    child_node->accept(*this);
    auto child_id = buffer_.back();
    buffer_.pop_back();
    auto edge = labeled_edge(parent_id, child_id, edge_name);
    of << edge;
    return child_id;
}

void GraphvizASMVisitor::visit_bin_exp(std::string node_name, ASM::BinInstructionNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        node_name,
        {}
    );
    visit_child(my_id, "left", node.left_);
    visit_child(my_id, "right", node.right_);
    buffer_.push_back(my_id);
}

void GraphvizASMVisitor::visit_un_exp(std::string node_name, ASM::UnaryInstructionNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        node_name,
        {}
    );
    visit_child(my_id, "operand", node.src_);
    buffer_.push_back(my_id);
}


void GraphvizASMVisitor::visit(ASM::ProgramNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        "ProgramNode",
        {}
    );
    of << node_repr;
    for(auto& fn : node.functions_) {
        visit_child(my_id, "function", fn);
    }
    buffer_.push_back(my_id);
}

void GraphvizASMVisitor::visit(ASM::FunctionNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        "FunctionNode",
        {std::make_pair("name", node.name_)}
    );
    of << node_repr;
    auto last_parent = my_id;
    for(auto& instruction : node.instructions_) {
        last_parent = visit_child(my_id, "next instruction", instruction);
    }
    buffer_.push_back(my_id);
}

// Unary exps
void GraphvizASMVisitor::visit(ASM::NegNode& node) {visit_un_exp("NegNode", node);}
void GraphvizASMVisitor::visit(ASM::NotNode& node) {visit_un_exp("NotNode", node);}

// binary arithmetic exps
void GraphvizASMVisitor::visit(ASM::MultNode& node) {visit_bin_exp("MultNode", node);}
void GraphvizASMVisitor::visit(ASM::AddNode& node) {visit_bin_exp("AddNode", node);}
void GraphvizASMVisitor::visit(ASM::SubNode& node) {visit_bin_exp("SubNode", node);}

// binary boolean exps
void GraphvizASMVisitor::visit(ASM::BitwiseAndNode& node) {visit_bin_exp("BitwiseAndNode", node);}
void GraphvizASMVisitor::visit(ASM::BitwiseOrNode& node) {visit_bin_exp("BitwiseOrNode", node);}
void GraphvizASMVisitor::visit(ASM::BitwiseXorNode& node) {visit_bin_exp("BitwiseXorNode", node);}
void GraphvizASMVisitor::visit(ASM::SalNode& node) {visit_bin_exp("SalNode", node);}
void GraphvizASMVisitor::visit(ASM::SarNode& node) {visit_bin_exp("SarNode", node);}

void GraphvizASMVisitor::visit(ASM::MovNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        "MovNode",
        {}
    );
    visit_child(my_id, "src", node.src_);
    visit_child(my_id, "dst", node.dst_);
    buffer_.push_back(my_id);
}

void GraphvizASMVisitor::visit(ASM::MovBNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        "MovBNode",
        {}
    );
    of << node_repr;
    visit_child(my_id, "src", node.src_);
    visit_child(my_id, "dst", node.dst_);
    buffer_.push_back(my_id);
}

void GraphvizASMVisitor::visit(ASM::DivNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        "DivNode",
        {}
    );
    visit_child(my_id, "src", node.src_);
}

void GraphvizASMVisitor::visit(ASM::CDQNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        "CDQNode",
        {}
    );
    of << node_repr;
    buffer_.push_back(my_id);
}

void GraphvizASMVisitor::visit(ASM::RetNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        "RetNode",
        {}
    );
    of << node_repr;
    buffer_.push_back(my_id);
}

void GraphvizASMVisitor::visit(ASM::AllocateStackNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        "AllocateStackNode",
        {std::make_pair("size", std::to_string(node.size_))}   
    );
    of << node_repr;
    buffer_.push_back(my_id);
}

void GraphvizASMVisitor::visit(ASM::ImmNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        "ImmNode",
        {std::make_pair("value", std::to_string(node.val_))}
    );
    of << node_repr;
    buffer_.push_back(my_id);
}

void GraphvizASMVisitor::visit(ASM::StackNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        "StackNode",
        {std::make_pair("size", std::to_string(node.size_))}
    );
    of << node_repr;
    buffer_.push_back(my_id);
}

void GraphvizASMVisitor::visit(ASM::RegisterNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        "RegisterNode",
        {std::make_pair("register", ASM::register_as_string(node.reg_))}
    );
    of << node_repr;
    buffer_.push_back(my_id);
}

void GraphvizASMVisitor::visit(ASM::PseudoNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        "PseudoNode",
        {std::make_pair("id", node.name_)}
    );
    of << node_repr;
    buffer_.push_back(my_id);
}


} //namespace Graphviz