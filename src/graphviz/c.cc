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

std::string GraphvizCAstVisitor::visit_child(std::string parent_id, std::string edge_label, std::shared_ptr<CAst::ASTNode> child_node) {
    child_node->accept(*this);
    auto child_id = buffer_.back();
    auto edge = labeled_edge(parent_id, child_id, edge_label);
    of << edge;
    buffer_.pop_back();
    return child_id;
}

void GraphvizCAstVisitor::visit_un_exp(std::string node_name, CAst::UnaryExpressionNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        node_name,
        {}
    );
    of << node_repr;
    visit_child(my_id, std::string("operand"), node.operand_);
    buffer_.push_back(my_id);
}

void GraphvizCAstVisitor::visit_bin_exp(std::string node_name, CAst::BinaryExpressionNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        node_name,
        {}
    );
    of << node_repr;

    visit_child(my_id, std::string("left"), node.left_);
    visit_child(my_id, std::string("right"), node.right_);

    buffer_.push_back(my_id);
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
        visit_child(my_id, std::string("function"), function);
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

    visit_child(my_id, "args", node.arguments_node_);
    visit_child(my_id, "stmts", node.body_);
    visit_child(my_id, "args", node.type_node_);

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

void GraphvizCAstVisitor::visit(CAst::BlockNode& node) {
    auto my_id = std::to_string(node_count_++);
    of << "  subgraph cluster_" << my_id << " {\n";
    of << "    label = \"Block Scope\";\n"; // Optional label for the box
    of << "    color = blue;\n";             // Box border color
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        "BlockNode",
        {}
    );
    of << node_repr;
    auto last_parent = my_id;
    for(auto& stmt : node.statements_) {
        last_parent = visit_child(last_parent, std::string("next statement"), stmt);
    }
    of << "  }\n";
    buffer_.push_back(my_id);
}

void GraphvizCAstVisitor::visit(CAst::NullNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        "NulLNode",
        {}
    );
    of << node_repr;
    buffer_.push_back(my_id);
}

void GraphvizCAstVisitor::visit(CAst::VariableNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        "VariableNode",
        {std::make_pair("name", node.name_)}
    );
    of << node_repr;
    buffer_.push_back(my_id);
}

void GraphvizCAstVisitor::visit(CAst::VariableDeclarationNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        "VariableDeclarationNode",
        {
            std::make_pair("name", node.var_->name_),
            std::make_pair("type",type_as_str(node.type_->type_))
        }
    );
    of << node_repr;
    if(node.expr_) {
        visit_child(my_id, "value expr", node.expr_.value());
    }
    buffer_.push_back(my_id);
}

void GraphvizCAstVisitor::visit(CAst::AssignmentNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        "AssignmentNode",
        {}
    );
    of << node_repr;
    visit_child(my_id, "assignee", node.left_);
    visit_child(my_id, "value", node.right_);
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
    visit_child(my_id, std::string("return value"), node.return_value_);
    buffer_.push_back(my_id);
}

void GraphvizCAstVisitor::visit(CAst::IfNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        "IfNode",
        {}
    );
    of << node_repr;
    visit_child(my_id, std::string("cond"), node.cond_);
    visit_child(my_id, std::string("then"), node.then_);
    if (node.alt_) {
        visit_child(my_id, std::string("else"), node.alt_.value());
    }
    buffer_.push_back(my_id);
}

void GraphvizCAstVisitor::visit(CAst::TernaryNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        "TernaryNode",
        {}
    );
    of << node_repr;
    visit_child(my_id, std::string("if"), node.cond_);
    visit_child(my_id, std::string("then"), node.then_);
    visit_child(my_id, std::string("else"), node.alt_);
    buffer_.push_back(my_id);
}

void GraphvizCAstVisitor::visit(CAst::DoWhileNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        "DoWhileNode",
        {std::make_pair("label", node.label_)}
    );
    of << node_repr;
    visit_child(my_id, "cond", node.cond_);
    visit_child(my_id, "cond", node.body_);
    buffer_.push_back(my_id);
}

void GraphvizCAstVisitor::visit(CAst::WhileNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        "WhileNode",
        {std::make_pair("label", node.label_)}
    );
    of << node_repr;
    visit_child(my_id, "cond", node.cond_);
    visit_child(my_id, "cond", node.body_);
    buffer_.push_back(my_id);
}

void GraphvizCAstVisitor::visit(CAst::ForNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        "ForNode",
        {std::make_pair("label", node.label_)}
    );
    of << node_repr;
    visit_child(my_id, "init", node.init_);
    if (node.cond_) {
        visit_child(my_id, "cond", node.cond_.value());
    }
    if (node.post_) {
        visit_child(my_id, "post", node.post_.value());
    }
    visit_child(my_id, "body", node.body_);
    buffer_.push_back(my_id);
}

void GraphvizCAstVisitor::visit(CAst::ContinueNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        "ContinueNode",
        {std::make_pair("label", node.label_)}
    );
    of << node_repr;
    buffer_.push_back(my_id);
}

void GraphvizCAstVisitor::visit(CAst::BreakNode& node) {
    auto my_id = std::to_string(node_count_++);
    auto node_repr = labeled_node_with_kv_pairs(
        my_id,
        "BreakNode",
        {std::make_pair("label", node.label_)}
    );
    of << node_repr;
    buffer_.push_back(my_id);
}

// Unary Expressions
void GraphvizCAstVisitor::visit(CAst::BitwiseNotUnaryExpressionNode& node) {visit_un_exp(std::string("BitwiseNotUnaryExpressionNode"), node);}
void GraphvizCAstVisitor::visit(CAst::NotUnaryExpressionNode& node) {visit_un_exp(std::string("NotUnaryExpressionNode"), node);}
void GraphvizCAstVisitor::visit(CAst::MinusUnaryExpressionNode& node) {visit_un_exp(std::string("MinusUnaryExpressionNode"), node);}

// Binary arithmetic Expressions
void GraphvizCAstVisitor::visit(CAst::DivNode& node) {visit_bin_exp("DivNode", node);}
void GraphvizCAstVisitor::visit(CAst::MultNode& node) {visit_bin_exp("MultNode", node);}
void GraphvizCAstVisitor::visit(CAst::ModNode& node) {visit_bin_exp("ModNode", node);}
void GraphvizCAstVisitor::visit(CAst::MinusNode& node) {visit_bin_exp("MinusNode", node);}
void GraphvizCAstVisitor::visit(CAst::PlusNode& node) {visit_bin_exp("PlusNode", node);}

// Binary bitwise expressions
void GraphvizCAstVisitor::visit(CAst::AndNode& node) {visit_bin_exp("AndNode", node);}
void GraphvizCAstVisitor::visit(CAst::OrNode& node) {visit_bin_exp("OrNode", node);}
void GraphvizCAstVisitor::visit(CAst::BitwiseAndNode& node) {visit_bin_exp("BitwiseAndNode", node);}
void GraphvizCAstVisitor::visit(CAst::BitwiseOrNode& node) {visit_bin_exp("BitwiseOrNode", node);}
void GraphvizCAstVisitor::visit(CAst::BitwiseXorNode& node) {visit_bin_exp("BitwiseXorNode", node);}
void GraphvizCAstVisitor::visit(CAst::BitwiseLeftShiftNode& node) {visit_bin_exp("BitwiseLeftShiftNode", node);}
void GraphvizCAstVisitor::visit(CAst::BitwiseRightShiftNode& node) {visit_bin_exp("BitwiseRightShiftNode", node);}

// Relational binary expressions
void GraphvizCAstVisitor::visit(CAst::EqualNode& node) {visit_bin_exp("EqualNode", node);}
void GraphvizCAstVisitor::visit(CAst::NotEqualNode& node) {visit_bin_exp("NotEqualNode", node);}
void GraphvizCAstVisitor::visit(CAst::LessNode& node) {visit_bin_exp("LessNode", node);}
void GraphvizCAstVisitor::visit(CAst::LessEqNode& node) {visit_bin_exp("LessEqNode", node);}
void GraphvizCAstVisitor::visit(CAst::GreaterNode& node) {visit_bin_exp("GreaterNode", node);}
void GraphvizCAstVisitor::visit(CAst::GreaterEqNode& node) {visit_bin_exp("GreaterEqNode", node);}

// Terminal nodes

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

} // namespace Graphviz