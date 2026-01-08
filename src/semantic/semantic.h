#ifndef _SEMANTIC_H_
#define _SEMANTIC_H_

#include "src/ast/c.h"
#include <vector>
#include <unordered_map>
#include <memory>
#include <optional>
namespace Semantic {

template<typename T>
inline std::shared_ptr<T> As(std::shared_ptr<CAst::ASTNode>& node) {
    auto casted_ptr = std::dynamic_pointer_cast<T>(node);
    if (!casted_ptr) {
        throw std::runtime_error("Unable to cast CAst::ASTNode to desired type");
    }
    return casted_ptr;
}

class SemanticAnalysisVisitor : public CAst::Visitor {
public:
    ~SemanticAnalysisVisitor() = default;
    SemanticAnalysisVisitor() = default;

    // the ones with actual sem analysis

    void visit(CAst::DeclarationNode& node) {
        auto var_name = node.var_->name_;

        if (lookup_symbol_current_scope(node.var_->name_)) {
            throw std::runtime_error("Variable already declared: " + var_name);
        }

        auto new_name = generate_unique_var_name();
        insert_symbol(node.var_->name_, new_name);

        std::optional<std::shared_ptr<CAst::ExpressionNode>> converted_expr = std::nullopt;
        if (node.expr_) {
            node.expr_.value()->accept(*this);
            converted_expr = As<CAst::ExpressionNode>(buffer_.back());
            buffer_.pop_back();
        }
        buffer_.push_back(std::make_shared<CAst::DeclarationNode>(
            std::make_shared<CAst::VariableNode>(new_name),
            node.type_,
            converted_expr
        ));
    }

    void visit(CAst::AssignmentNode& node) {
        node.left_->accept(*this);
        auto casted_left = As<CAst::ExpressionNode>(buffer_.back());
        buffer_.pop_back();
        auto left_as_var = std::dynamic_pointer_cast<CAst::VariableNode>(casted_left);
        if (!left_as_var) throw std::runtime_error("Can only assign to a variable, invalid lvalue");
        node.right_->accept(*this);
        auto casted_right = As<CAst::ExpressionNode>(buffer_.back());
        buffer_.pop_back();
        buffer_.push_back(std::make_shared<CAst::AssignmentNode>(
            casted_left,
            casted_right
        ));
    }

    void visit(CAst::VariableNode& node) {
        auto lookup = lookup_symbol(node.name_);
        if(!lookup) throw std::runtime_error("Variable referenced before assignment: " + node.name_);
        buffer_.push_back(std::make_shared<CAst::VariableNode>(lookup.value()));
    }

    void visit(CAst::ProgramNode& node) {
        std::vector<std::shared_ptr<CAst::FunctionNode>> functions;
        for (auto fn : node.functions_) {
            fn->accept(*this);
            auto processed_fn = As<CAst::FunctionNode>(buffer_.back());
            functions.push_back(processed_fn);
            buffer_.pop_back();
        }
        buffer_.push_back(std::make_shared<CAst::ProgramNode>(std::move(functions)));
    }

    void visit(CAst::FunctionNode& node) {
        node.arguments_node_->accept(*this);
        auto args = As<CAst::FunctionArgumentsNode>(buffer_.back());
        buffer_.pop_back();
        symbol_table_.emplace_back();
        node.body_->accept(*this);
        symbol_table_.pop_back();
        auto body = As<CAst::BlockNode>(buffer_.back());
        buffer_.pop_back();
        buffer_.push_back(std::make_shared<CAst::FunctionNode>(
            node.name_,
            node.type_node_,
            args,
            body
        ));
    }

    void visit(CAst::TypeNode& node) {
        buffer_.push_back(std::make_shared<CAst::TypeNode>(node.type_));
    }

    void visit(CAst::FunctionArgumentsNode& node) {
        buffer_.push_back(std::make_shared<CAst::FunctionArgumentsNode>(std::move(node.arguments_)));
    }

    void visit(CAst::BlockNode& node) {
        std::vector<std::shared_ptr<CAst::BlockElementNode>> els;
        for(auto& el : node.statements_) {
            el->accept(*this);
            auto processed_el = As<CAst::BlockElementNode>(buffer_.back());
            buffer_.pop_back();
            els.push_back(processed_el);
        }
        buffer_.push_back(std::make_shared<CAst::BlockNode>(
            std::move(els)
        ));
    }

    void visit(CAst::ReturnStatementNode& node) {
        node.return_value_->accept(*this);
        auto processed_expr = As<CAst::ExpressionNode>(buffer_.back());
        buffer_.pop_back();
        buffer_.push_back(std::make_shared<CAst::ReturnStatementNode>(node.type_, processed_expr));
    }

    void visit(CAst::NullNode& node) {buffer_.push_back(std::make_shared<CAst::NullNode>());}

    // unary exps

    template<std::derived_from<CAst::UnaryExpressionNode> T>
    void visit_unexp(CAst::UnaryExpressionNode& node) {
        node.operand_->accept(*this);
        auto processed_operand = As<CAst::ExpressionNode>(buffer_.back());
        buffer_.pop_back();
        buffer_.push_back(std::make_shared<T>(processed_operand));
    }

    void visit(CAst::BitwiseNotUnaryExpressionNode& node) {visit_unexp<CAst::BitwiseNotUnaryExpressionNode>(node);}
    void visit(CAst::MinusUnaryExpressionNode& node) {visit_unexp<CAst::MinusUnaryExpressionNode>(node);}
    void visit(CAst::NotUnaryExpressionNode& node) {visit_unexp<CAst::NotUnaryExpressionNode>(node);}


    // bin exps

    template<std::derived_from<CAst::BinaryExpressionNode> T>
    void visit_binexp(CAst::BinaryExpressionNode& node) {
        node.left_->accept(*this);
        auto processed_left = As<CAst::ExpressionNode>(buffer_.back());
        buffer_.pop_back();

        node.right_->accept(*this);
        auto processed_right = As<CAst::ExpressionNode>(buffer_.back());
        buffer_.pop_back();

        buffer_.push_back(std::make_shared<T>(processed_left, processed_right));
    }

    void visit(CAst::AndNode& node) {visit_binexp<CAst::AndNode>(node);}
    void visit(CAst::OrNode& node) {visit_binexp<CAst::OrNode>(node);}
    void visit(CAst::BitwiseXorNode& node) {visit_binexp<CAst::BitwiseXorNode>(node);}
    void visit(CAst::BitwiseAndNode& node) {visit_binexp<CAst::BitwiseAndNode>(node);}
    void visit(CAst::BitwiseOrNode& node) {visit_binexp<CAst::BitwiseOrNode>(node);}
    void visit(CAst::BitwiseLeftShiftNode& node) {visit_binexp<CAst::BitwiseLeftShiftNode>(node);}
    void visit(CAst::BitwiseRightShiftNode& node) {visit_binexp<CAst::BitwiseRightShiftNode>(node);}

    void visit(CAst::EqualNode& node) {visit_binexp<CAst::EqualNode>(node);}
    void visit(CAst::NotEqualNode& node) {visit_binexp<CAst::NotEqualNode>(node);}
    void visit(CAst::GreaterNode& node) {visit_binexp<CAst::GreaterNode>(node);}
    void visit(CAst::GreaterEqNode& node) {visit_binexp<CAst::GreaterEqNode>(node);}
    void visit(CAst::LessNode& node) {visit_binexp<CAst::LessNode>(node);}
    void visit(CAst::LessEqNode& node) {visit_binexp<CAst::LessEqNode>(node);}

    void visit(CAst::ModNode& node) {visit_binexp<CAst::ModNode>(node);}
    void visit(CAst::DivNode& node) {visit_binexp<CAst::DivNode>(node);}
    void visit(CAst::MultNode& node) {visit_binexp<CAst::MultNode>(node);}
    void visit(CAst::PlusNode& node) {visit_binexp<CAst::PlusNode>(node);}
    void visit(CAst::MinusNode& node) {visit_binexp<CAst::MinusNode>(node);}

    void visit(CAst::IntegerValueNode& node) {
        buffer_.push_back(std::make_shared<CAst::IntegerValueNode>(node.value_));
    }

    void visit(CAst::IfNode& node) {
        node.cond_->accept(*this);
        auto processed_cond = As<CAst::ExpressionNode>(buffer_.back());
        buffer_.pop_back();
        node.then_->accept(*this);
        auto processed_then = As<CAst::StatementNode>(buffer_.back());
        buffer_.pop_back();
        if (!node.alt_) {
            buffer_.push_back(
                std::make_shared<CAst::IfNode>(
                    processed_cond,
                    processed_then
            ));
            return;
        }
        node.alt_.value()->accept(*this);
        auto processed_alt = As<CAst::StatementNode>(buffer_.back());
        buffer_.pop_back();
            buffer_.push_back(
                std::make_shared<CAst::IfNode>(
                    processed_cond,
                    processed_then,
                    processed_alt
            ));
    }

    void visit(CAst::TernaryNode& node) {
        node.cond_->accept(*this);
        auto processed_cond = As<CAst::ExpressionNode>(buffer_.back());
        buffer_.pop_back();
        node.then_->accept(*this);
        auto processed_then = As<CAst::ExpressionNode>(buffer_.back());
        buffer_.pop_back();
        node.alt_->accept(*this);
        auto processed_alt = As<CAst::ExpressionNode>(buffer_.back());
        buffer_.pop_back();
        buffer_.push_back(std::make_shared<CAst::TernaryNode>(
            processed_cond,
            processed_then,
            processed_alt
        ));
    }

    void visit(CAst::ForNode& node) {
        symbol_table_.emplace_back();

        node.init_->accept(*this);
        auto processed_init = As<CAst::BlockElementNode>(buffer_.back());
        buffer_.pop_back();

        node.body_->accept(*this);
        auto processed_body = As<CAst::StatementNode>(buffer_.back());
        buffer_.pop_back();

        std::optional<std::shared_ptr<CAst::ExpressionNode>> processed_cond = std::nullopt;
        if (node.cond_) {
            node.cond_.value()->accept(*this);
            processed_cond = As<CAst::ExpressionNode>(buffer_.back());
            buffer_.pop_back();
        }
        std::optional<std::shared_ptr<CAst::ExpressionNode>> processed_post = std::nullopt;
        if (node.post_) {
            node.post_.value()->accept(*this);
            processed_post = As<CAst::ExpressionNode>(buffer_.back());
            buffer_.pop_back();
        }

        symbol_table_.pop_back();

        buffer_.push_back(
            std::make_shared<CAst::ForNode>(
                processed_init,
                processed_cond,
                processed_post,
                processed_body,
                node.label_
            )
        );

    }

    void visit(CAst::DoWhileNode& node) {
        node.cond_->accept(*this);
        auto processed_cond = As<CAst::ExpressionNode>(buffer_.back());
        buffer_.pop_back();
        node.body_->accept(*this);
        auto processed_body = As<CAst::StatementNode>(buffer_.back());
        buffer_.pop_back();
        buffer_.push_back(std::make_shared<CAst::DoWhileNode>(
           processed_cond,
           processed_body,
           node.label_ 
        ));
    }

    void visit(CAst::WhileNode& node) {
        node.cond_->accept(*this);
        auto processed_cond = As<CAst::ExpressionNode>(buffer_.back());
        buffer_.pop_back();
        node.body_->accept(*this);
        auto processed_body = As<CAst::StatementNode>(buffer_.back());
        buffer_.pop_back();
        buffer_.push_back(std::make_shared<CAst::WhileNode>(
           processed_cond,
           processed_body,
           node.label_ 
        ));
    }

    void visit(CAst::BreakNode& node) {
        buffer_.push_back(std::make_shared<CAst::BreakNode>(node.label_));
    }

    void visit(CAst::ContinueNode& node) {
        buffer_.push_back(std::make_shared<CAst::BreakNode>(node.label_));
    }

    std::shared_ptr<CAst::ProgramNode> process(CAst::ProgramNode& node) {
        node.accept(*this);
        if (buffer_.size() != 1) {
            throw std::runtime_error("Expected exactly one element in the buffer after semantic analysis.");
        }
        auto program = As<CAst::ProgramNode>(buffer_.back());
        buffer_.pop_back();
        return program;
    }

private:
    std::vector<std::unordered_map<std::string, std::string>> symbol_table_;
    int var_counter_;
    std::vector<std::shared_ptr<CAst::ASTNode>> buffer_;
    std::optional<std::string> lookup_symbol_current_scope(std::string name) {
        if (symbol_table_.empty()) {return std::nullopt;}
        auto& m = symbol_table_.back();
        auto entry = m.find(name);
        if (entry == m.end())   return std::nullopt;
        return entry->second;
    }
    std::optional<std::string> lookup_symbol(std::string name) {
        if (symbol_table_.empty()) {return std::nullopt;}
        for(auto m = symbol_table_.rbegin(); m != symbol_table_.rend(); m++) {
            auto entry = m->find(name);
            if (entry == m->end()) continue;
            return entry->second;
        }
        return std::nullopt;
    }
    void insert_symbol(std::string key, std::string value) {
        if(symbol_table_.empty()) throw std::runtime_error("Cannot insert into a symbol table with an empty map stack.");
        auto& m = symbol_table_.back();
        m[key] = value;
    }
    std::string generate_unique_var_name() {
        return "$_var_" +  std::to_string(var_counter_++);
    }
};


} // namespace Semantic
#endif // _SEMANTIC_H