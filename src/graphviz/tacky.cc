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

void GraphvizTackyVisitor::visit(Tacky::ProgramNode& node) {
    auto my_id = std::to_string(node_count_++);
    std::string node_repr = labeled_node_with_kv_pairs(
        my_id,
        "ProgramNode",
        {}
    );
    of << node_repr;
    for (auto &fn : node.functions_) {
        fn->accept(*this);
        auto edge = labeled_edge(my_id, buffer_.back(), "function");
        of << edge;
        buffer_.pop_back();
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
        instruction->accept(*this);
        auto child_id = buffer_.back();
        buffer_.pop_back();
        auto edge = labeled_edge(last_parent, child_id, "next instruction");
        of << edge;
        last_parent = child_id;
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
    node.value_->accept(*this);
    auto child_id = buffer_.back();
    buffer_.pop_back();
    auto edge = labeled_edge(my_id, child_id, "return value");
    of << edge;
    buffer_.push_back(my_id);
}

void GraphvizTackyVisitor::visit(Tacky::ComplementNode& node) {
    auto my_id = std::to_string(node_count_++);
    std::string node_repr = labeled_node_with_kv_pairs(
        my_id,
        "ComplementNode",
        {}
    );
    of << node_repr;
    node.src_->accept(*this);
    auto src_id = buffer_.back();
    buffer_.pop_back();
    auto src_edge = labeled_edge(my_id, src_id, "src");
    node.dst_->accept(*this);
    auto dst_id = buffer_.back();
    buffer_.pop_back();
    auto dst_edge = labeled_edge(my_id, dst_id, "dst");
    of << src_edge;
    of << dst_edge;
    buffer_.push_back(my_id);
}

void GraphvizTackyVisitor::visit(Tacky::NegateNode& node) {
    //todo: move logic to father class
    auto my_id = std::to_string(node_count_++);
    std::string node_repr = labeled_node_with_kv_pairs(
        my_id,
        "NegateNode",
        {}
    );
    of << node_repr;
    node.src_->accept(*this);
    auto src_id = buffer_.back();
    buffer_.pop_back();
    auto src_edge = labeled_edge(my_id, src_id, "src");
    node.dst_->accept(*this);
    auto dst_id = buffer_.back();
    buffer_.pop_back();
    auto dst_edge = labeled_edge(my_id, dst_id, "dst");
    of << src_edge;
    of << dst_edge;
    buffer_.push_back(my_id);
}

void GraphvizTackyVisitor::visit(Tacky::DivNode& node) {
    auto my_id = std::to_string(node_count_++);
    std::string node_repr = labeled_node_with_kv_pairs(
        my_id,
        "DivNode",
        {}
    );
    of << node_repr;

    node.left_->accept(*this);
    auto left_id = buffer_.back();
    buffer_.pop_back();
    auto left_edge = labeled_edge(my_id, left_id, "left");

    node.right_->accept(*this);
    auto right_id = buffer_.back();
    buffer_.pop_back();
    auto right_edge = labeled_edge(my_id, right_id, "right");

    node.dst_->accept(*this);
    auto dst_id = buffer_.back();
    buffer_.pop_back();
    auto dst_edge = labeled_edge(my_id, dst_id, "dst");
    of << left_edge;
    of << right_edge;
    of << dst_edge;
    buffer_.push_back(my_id);
}

void GraphvizTackyVisitor::visit(Tacky::MultNode& node) {
    auto my_id = std::to_string(node_count_++);
    std::string node_repr = labeled_node_with_kv_pairs(
        my_id,
        "MultNode",
        {}
    );
    of << node_repr;

    node.left_->accept(*this);
    auto left_id = buffer_.back();
    buffer_.pop_back();
    auto left_edge = labeled_edge(my_id, left_id, "left");

    node.right_->accept(*this);
    auto right_id = buffer_.back();
    buffer_.pop_back();
    auto right_edge = labeled_edge(my_id, right_id, "right");

    node.dst_->accept(*this);
    auto dst_id = buffer_.back();
    buffer_.pop_back();
    auto dst_edge = labeled_edge(my_id, dst_id, "dst");
    of << left_edge;
    of << right_edge;
    of << dst_edge;
    buffer_.push_back(my_id);
}

void GraphvizTackyVisitor::visit(Tacky::ModNode& node) {
    auto my_id = std::to_string(node_count_++);
    std::string node_repr = labeled_node_with_kv_pairs(
        my_id,
        "ModNode",
        {}
    );
    of << node_repr;

    node.left_->accept(*this);
    auto left_id = buffer_.back();
    buffer_.pop_back();
    auto left_edge = labeled_edge(my_id, left_id, "left");

    node.right_->accept(*this);
    auto right_id = buffer_.back();
    buffer_.pop_back();
    auto right_edge = labeled_edge(my_id, right_id, "right");

    node.dst_->accept(*this);
    auto dst_id = buffer_.back();
    buffer_.pop_back();
    auto dst_edge = labeled_edge(my_id, dst_id, "dst");
    of << left_edge;
    of << right_edge;
    of << dst_edge;
    buffer_.push_back(my_id);
}

void GraphvizTackyVisitor::visit(Tacky::PlusNode& node) {
    auto my_id = std::to_string(node_count_++);
    std::string node_repr = labeled_node_with_kv_pairs(
        my_id,
        "PlusNode",
        {}
    );
    of << node_repr;

    node.left_->accept(*this);
    auto left_id = buffer_.back();
    buffer_.pop_back();
    auto left_edge = labeled_edge(my_id, left_id, "left");

    node.right_->accept(*this);
    auto right_id = buffer_.back();
    buffer_.pop_back();
    auto right_edge = labeled_edge(my_id, right_id, "right");

    node.dst_->accept(*this);
    auto dst_id = buffer_.back();
    buffer_.pop_back();
    auto dst_edge = labeled_edge(my_id, dst_id, "dst");
    of << left_edge;
    of << right_edge;
    of << dst_edge;
    buffer_.push_back(my_id);
}

void GraphvizTackyVisitor::visit(Tacky::MinusNode& node) {
    auto my_id = std::to_string(node_count_++);
    std::string node_repr = labeled_node_with_kv_pairs(
        my_id,
        "MinusNode",
        {}
    );
    of << node_repr;

    node.left_->accept(*this);
    auto left_id = buffer_.back();
    buffer_.pop_back();
    auto left_edge = labeled_edge(my_id, left_id, "left");

    node.right_->accept(*this);
    auto right_id = buffer_.back();
    buffer_.pop_back();
    auto right_edge = labeled_edge(my_id, right_id, "right");

    node.dst_->accept(*this);
    auto dst_id = buffer_.back();
    buffer_.pop_back();
    auto dst_edge = labeled_edge(my_id, dst_id, "dst");
    of << left_edge;
    of << right_edge;
    of << dst_edge;
    buffer_.push_back(my_id);
}


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