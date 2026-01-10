#include "src/semantic/semantic.h"
#include <ranges>

namespace Semantic {

std::string VariableResolutionVisitor::generate_unique_var_name() {
    return "$_var_" +  std::to_string(var_counter_++);
}

void VariableResolutionVisitor::new_decl(std::string name, CAst::Type type) {
    if (lookup_symbol_current_scope(name)) {
        throw std::runtime_error("Symbol already declared: " + name);
    }

    auto new_name = generate_unique_var_name();
    SymbolEntry new_symbol = SymbolEntry{
        new_name,
        is_on_file_scope,
        type,
        std::monostate{}
    };
    insert_symbol(name, new_symbol);
}

void VariableResolutionVisitor::visit(CAst::FunctionCallNode& node) {
    auto entry = lookup_symbol(node.name_);

    if (!entry) {
        throw std::runtime_error("Cannot call undeclared function: " + node.name_);
    }

    if(entry->type != CAst::Type::FUNCTION) {
        throw std::runtime_error("Cannot call symbol that does not map to a function: " + node.name_);
    }

    auto metadata = std::get<FunctionMetadata>(entry->metadata);
    auto args_count = node.args_.size();
    auto registered_args_count = metadata.arg_types.size();

    if (args_count != registered_args_count) {
        throw std::runtime_error("Mismatching arg count: expected" + std::to_string(args_count) + ", got " + std::to_string(registered_args_count));
    }

    std::vector<std::shared_ptr<CAst::ExpressionNode>> new_args;
    for(auto arg : node.args_) {
        arg->accept(*this);
        new_args.push_back(As<CAst::ExpressionNode>(buffer_.back()));
        buffer_.pop_back();
    }

    buffer_.push_back(std::make_shared<CAst::FunctionCallNode>(node.name_, std::move(new_args)));
}

void VariableResolutionVisitor::visit(CAst::VariableDeclarationNode& node) {
    auto var_name = node.var_->name_;

    new_decl(var_name, node.type_->type_);

    std::optional<std::shared_ptr<CAst::ExpressionNode>> converted_expr = std::nullopt;
    if (node.expr_) {
        node.expr_.value()->accept(*this);
        converted_expr = As<CAst::ExpressionNode>(buffer_.back());
        buffer_.pop_back();
    }

    auto new_entry = lookup_symbol_current_scope(node.var_->name_);
    auto new_name = new_entry.value().name;
    buffer_.push_back(std::make_shared<CAst::VariableDeclarationNode>(
        std::make_shared<CAst::VariableNode>(new_name),
        node.type_,
        converted_expr
    ));
}

void VariableResolutionVisitor::visit(CAst::VariableNode& node) {
    auto lookup = lookup_symbol(node.name_);
    if(!lookup) throw std::runtime_error("Variable referenced before assignment: " + node.name_);
    if (lookup->type == CAst::Type::FUNCTION) {
        throw std::runtime_error("Cannot use function as a variable");
    }
    buffer_.push_back(std::make_shared<CAst::VariableNode>(lookup.value().name));
}

void VariableResolutionVisitor::visit(CAst::FunctionNode& node) {
    if (is_within_function_ && node.body_.has_value()) {
        throw std::runtime_error("Cannot declared nested functions.");
    }
    is_within_function_ = true;

    auto local_entry = lookup_symbol_current_scope(node.name_);
    if (local_entry && !local_entry->has_external_linkage) {
        throw std::runtime_error("Symbol already declared in local scope without external linkage: " + node.name_);
    }

    auto entry = lookup_symbol(node.name_);

    if (entry &&
        node.body_.has_value() &&
        entry->type == CAst::FUNCTION &&
        std::get<FunctionMetadata>(entry->metadata).was_defined) {
        throw std::runtime_error("Function was already defined: " + node.name_);
    }
    
    std::vector<CAst::Type> args;
    for(auto arg : node.arguments_node_->arguments_) {
        args.push_back(arg.type);
    }
    if (entry) {
        auto old_args = std::get<FunctionMetadata>(entry->metadata).arg_types;
        if (entry && old_args.size() != args.size()) {
            throw std::runtime_error("Previous function declaration has incompatible number of arguments: expected " + std::to_string(old_args.size()) + ", got " + std::to_string(args.size()));
        }

        for (auto const& [old_arg, new_arg] : std::views::zip(old_args, args)) {
            if (old_arg != new_arg) {
                throw std::runtime_error("Argument mismatch from previous declaration: expected " + CAst::type_as_str(old_arg) + ", got " + CAst::type_as_str(new_arg));
            }
        }
    }

    entry = SymbolEntry {
        node.name_,
        true,
        CAst::Type::FUNCTION,
        FunctionMetadata{
            node.body_.has_value(),
            std::move(args)
        }
    };
    insert_symbol(node.name_, entry.value());

    symbol_table_.emplace_back();

    std::list<CAst::FunctionArgument> new_args;
    for(auto arg : node.arguments_node_->arguments_) {
        new_decl(arg.name, arg.type);
        new_args.push_back(CAst::FunctionArgument{
            arg.type,
            lookup_symbol_current_scope(arg.name)->name
        });
    }

    std::optional<std::shared_ptr<CAst::BlockNode>> new_block = std::nullopt;
    if (node.body_) {
        node.body_.value()->accept(*this);
        new_block = As<CAst::BlockNode>(buffer_.back());
        buffer_.pop_back();
    }

    symbol_table_.pop_back();

    is_within_function_ = false;

    buffer_.push_back(std::make_shared<CAst::FunctionNode>(
        node.name_,
        node.type_node_,
        std::make_shared<CAst::FunctionArgumentsNode>(std::move(new_args)),
        new_block
    ));
}

std::shared_ptr<CAst::ProgramNode> VariableResolutionVisitor::process(CAst::ProgramNode& node) {
    node.accept(*this);
    if (buffer_.size() != 1) {
        throw std::runtime_error("Expected exactly one element in the buffer after semantic analysis.");
    }
    auto program = As<CAst::ProgramNode>(buffer_.back());
    buffer_.pop_back();
    return program;
}

} // namespace Semantic