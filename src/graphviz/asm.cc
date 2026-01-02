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

void GraphvizASMVisitor::visit(ASM::ProgramNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        "ProgramNode",
        {}
    );
    of << node_repr;
    for(auto& fn : node.functions_) {
        fn->accept(*this);
        auto child_id = buffer_.back();
        buffer_.pop_back();
        auto edge = labeled_edge(my_id, child_id, "function");
        of << edge;
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
        instruction->accept(*this);
        auto child_id = buffer_.back();
        buffer_.pop_back();
        auto edge = labeled_edge(last_parent, child_id, "next instruction");
        of << edge;
        last_parent = child_id;
    }
    buffer_.push_back(my_id);
}

void GraphvizASMVisitor::visit(ASM::NegNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        "NegNode",
        {}
    );
    of << node_repr;
    node.src_->accept(*this);
    auto child_id = buffer_.back();
    buffer_.pop_back();
    auto edge = labeled_edge(my_id, child_id, "operand");
    of << edge;
    buffer_.push_back(my_id);
}

void GraphvizASMVisitor::visit(ASM::NotNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        "NotNode",
        {}
    );
    of << node_repr;
    node.src_->accept(*this);
    auto child_id = buffer_.back();
    buffer_.pop_back();
    auto edge = labeled_edge(my_id, child_id, "operand");
    of << edge;
    buffer_.push_back(my_id);
}

void GraphvizASMVisitor::visit(ASM::MovNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        "MovNode",
        {}
    );
    of << node_repr;
    node.src_->accept(*this);
    auto src_id = buffer_.back();
    buffer_.pop_back();
    node.dst_->accept(*this);
    auto dst_id = buffer_.back();
    buffer_.pop_back();
    auto src_edge = labeled_edge(my_id, src_id, "src");
    of << src_edge;
    auto dst_edge = labeled_edge(my_id, dst_id, "dst");
    of << dst_edge;
    buffer_.push_back(my_id);
}

void GraphvizASMVisitor::visit(ASM::DivNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        "DivNode",
        {}
    );
    of << node_repr;
    node.src_->accept(*this);
    auto operand_id = buffer_.back();
    buffer_.pop_back();
    auto operand_edge = labeled_edge(my_id, operand_id, "src");
    of << operand_edge;
    buffer_.push_back(my_id);
}

void GraphvizASMVisitor::visit(ASM::MultNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        "MultNode",
        {}
    );
    of << node_repr;
    node.left_->accept(*this);
    auto left_id = buffer_.back();
    buffer_.pop_back();
    node.right_->accept(*this);
    auto right_id = buffer_.back();
    buffer_.pop_back();
    auto left_edge = labeled_edge(my_id, left_id, "left");
    auto right_edge = labeled_edge(my_id, right_id, "right");
    of << left_edge;
    of << right_edge;
    buffer_.push_back(my_id);
}

void GraphvizASMVisitor::visit(ASM::AddNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        "AddNode",
        {}
    );
    of << node_repr;
    node.left_->accept(*this);
    auto left_id = buffer_.back();
    buffer_.pop_back();
    node.right_->accept(*this);
    auto right_id = buffer_.back();
    buffer_.pop_back();
    auto left_edge = labeled_edge(my_id, left_id, "left");
    auto right_edge = labeled_edge(my_id, right_id, "right");
    of << left_edge;
    of << right_edge;
    buffer_.push_back(my_id);
}

void GraphvizASMVisitor::visit(ASM::SubNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        "SubNode",
        {}
    );
    of << node_repr;
    node.left_->accept(*this);
    auto left_id = buffer_.back();
    buffer_.pop_back();
    node.right_->accept(*this);
    auto right_id = buffer_.back();
    buffer_.pop_back();
    auto left_edge = labeled_edge(my_id, left_id, "left");
    auto right_edge = labeled_edge(my_id, right_id, "right");
    of << left_edge;
    of << right_edge;
    buffer_.push_back(my_id);
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