#include "src/graphviz/graphviz.h"
#include <stdexcept>

namespace Graphviz {

GraphvizCAstVisitor::GraphvizCAstVisitor(std::string filename) : node_count_(0) {
    of = std::ofstream(filename);
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

void GraphvizCAstVisitor::visit(CAst::ProgramNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        "ProgramNode",
        {}
    );
    of << node_repr;
    for(auto& function: node.functions_) {
        function->accept(*this);
        auto child_id = buffer_.back();
        buffer_.pop_back();
        auto edge = labeled_edge(my_id, child_id, "function");
        of << edge;
    }
    buffer_.push_back(my_id);
}

void GraphvizCAstVisitor::visit(CAst::FunctionNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        "FunctionNode",
        {std::make_pair("Name", node.name_)}
    );
    of << node_repr;
    std::vector<std::shared_ptr<CAst::ASTNode>> children = {
        node.arguments_node_,
        node.body_,
        node.type_node_
    };
    for(auto& child : children) {
        child->accept(*this);
        auto child_id = buffer_.back();
        buffer_.pop_back();
        auto edge = labeled_edge(my_id, child_id, "");
        of << edge;
    }
    buffer_.push_back(my_id);
}

void GraphvizCAstVisitor::visit(CAst::TypeNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        "TypeNode",
        {std::make_pair("type", type_as_str(node.type_))}
    );
    of << node_repr;
    buffer_.push_back(my_id);
}

void GraphvizCAstVisitor::visit(CAst::FunctionArgumentsNode& node) {
    auto my_id = std::to_string(node_count_++);
    std::vector<NodeKVPair> kvpairs;
    for(auto x : node.arguments_) {
        kvpairs.push_back(std::make_pair(type_as_str(x.type), x.name));
    }
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        "FunctionArgsNode",
        std::move(kvpairs)
    );
    of << node_repr;
    buffer_.push_back(my_id);
}

void GraphvizCAstVisitor::visit(CAst::StatementBlockNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        "StatementBlockNode",
        {}
    );
    of << node_repr;
    auto last_parent = my_id;
    for(auto& stmt : node.statements_) {
        stmt->accept(*this);
        auto child_id = buffer_.back();
        buffer_.pop_back();
        auto edge = labeled_edge(last_parent, child_id, "next statement");
        of << edge;
        last_parent = child_id;
    }
    buffer_.push_back(my_id);
}


void GraphvizCAstVisitor::visit(CAst::ReturnStatementNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        "ReturnStatementNode",
        {}
    );
    of << node_repr;
    node.return_value_->accept(*this);
    auto child_id = buffer_.back();
    buffer_.pop_back();
    auto edge = labeled_edge(my_id, child_id, "return value");
    of << edge;
    buffer_.push_back(my_id);
}

void GraphvizCAstVisitor::visit(CAst::TildeUnaryExpressionNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        "TildeUnaryExpressionNode",
        {}
    );
    of << node_repr;
    node.operand_->accept(*this);
    auto child_id = buffer_.back();
    buffer_.pop_back();
    auto edge = labeled_edge(my_id, child_id, "operand");
    of << edge;
    buffer_.push_back(my_id);
}

void GraphvizCAstVisitor::visit(CAst::MinusUnaryExpressionNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        "MinusUnaryExpressionNode",
        {}
    );
    of << node_repr;
    node.operand_->accept(*this);
    auto child_id = buffer_.back();
    buffer_.pop_back();
    auto edge = labeled_edge(my_id, child_id, "operand");
    of << edge;
    buffer_.push_back(my_id);
}

void GraphvizCAstVisitor::visit(CAst::DivNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        "DivNode",
        {}
    );
    of << node_repr;

    node.left_->accept(*this);
    auto left_child_id = buffer_.back();
    buffer_.pop_back();
    auto edge = labeled_edge(my_id, left_child_id, "left");
    of << edge;

    node.right_->accept(*this);
    auto right_child_id = buffer_.back();
    buffer_.pop_back();
    edge = labeled_edge(my_id, right_child_id, "right");
    of << edge;
    buffer_.push_back(my_id);
}

void GraphvizCAstVisitor::visit(CAst::MultNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        "MultNode",
        {}
    );
    of << node_repr;

    node.left_->accept(*this);
    auto left_child_id = buffer_.back();
    buffer_.pop_back();
    auto edge = labeled_edge(my_id, left_child_id, "left");
    of << edge;

    node.right_->accept(*this);
    auto right_child_id = buffer_.back();
    buffer_.pop_back();
    edge = labeled_edge(my_id, right_child_id, "right");
    of << edge;
    buffer_.push_back(my_id);
}

void GraphvizCAstVisitor::visit(CAst::ModNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        "ModNode",
        {}
    );
    of << node_repr;

    node.left_->accept(*this);
    auto left_child_id = buffer_.back();
    buffer_.pop_back();
    auto edge = labeled_edge(my_id, left_child_id, "left");
    of << edge;

    node.right_->accept(*this);
    auto right_child_id = buffer_.back();
    buffer_.pop_back();
    edge = labeled_edge(my_id, right_child_id, "right");
    of << edge;
    buffer_.push_back(my_id);
}

void GraphvizCAstVisitor::visit(CAst::MinusNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        "MinusNode",
        {}
    );
    of << node_repr;

    node.left_->accept(*this);
    auto left_child_id = buffer_.back();
    buffer_.pop_back();
    auto edge = labeled_edge(my_id, left_child_id, "left");
    of << edge;

    node.right_->accept(*this);
    auto right_child_id = buffer_.back();
    buffer_.pop_back();
    edge = labeled_edge(my_id, right_child_id, "right");
    of << edge;
    buffer_.push_back(my_id);
}

void GraphvizCAstVisitor::visit(CAst::PlusNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        "PlusNode",
        {}
    );
    of << node_repr;

    node.left_->accept(*this);
    auto left_child_id = buffer_.back();
    buffer_.pop_back();
    auto edge = labeled_edge(my_id, left_child_id, "left");
    of << edge;

    node.right_->accept(*this);
    auto right_child_id = buffer_.back();
    buffer_.pop_back();
    edge = labeled_edge(my_id, right_child_id, "right");
    of << edge;
    buffer_.push_back(my_id);
}

void GraphvizCAstVisitor::visit(CAst::IntegerValueNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        "IntegerNode",
        {std::make_pair("Value", std::to_string(node.value_))}
    );
    of << node_repr;
    buffer_.push_back(my_id);
}

} // namespace CAst