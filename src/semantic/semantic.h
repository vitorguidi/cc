#ifndef _SEMANTIC_H_
#define _SEMANTIC_H_

#include "src/ast/c.h"
#include <vector>
#include <unordered_map>
#include <memory>
#include <optional>
#include <variant>

namespace Semantic {

struct FunctionMetadata {
    bool was_defined;
    std::vector<CAst::Type> arg_types;
};

typedef std::variant<FunctionMetadata, std::monostate> SymbolMetadata;

struct SymbolEntry {
    std::string name;
    bool has_external_linkage;
    CAst::Type type;
    SymbolMetadata metadata;
};

template<typename T>
inline std::shared_ptr<T> As(std::shared_ptr<CAst::ASTNode>& node) {
    auto casted_ptr = std::dynamic_pointer_cast<T>(node);
    if (!casted_ptr) {
        throw std::runtime_error("Unable to cast CAst::ASTNode to desired type");
    }
    return casted_ptr;
}

class SemanticCAstRewriter : public CAst::Visitor {
public:
    ~SemanticCAstRewriter() = default;
    SemanticCAstRewriter() = default;
    void visit(CAst::TypeNode& node) override;
    void visit(CAst::FunctionArgumentsNode& node) override;
    void visit(CAst::ReturnStatementNode& node) override;
    void visit(CAst::BlockNode& node) override;
    void visit(CAst::FunctionNode& node) override;
    void visit(CAst::BitwiseNotUnaryExpressionNode& node) override;
    void visit(CAst::MinusUnaryExpressionNode& node) override;
    void visit(CAst::NotUnaryExpressionNode& node) override;
    void visit(CAst::AndNode& node) override;
    void visit(CAst::OrNode& node) override;
    void visit(CAst::BitwiseXorNode& node) override;
    void visit(CAst::BitwiseAndNode& node) override;
    void visit(CAst::BitwiseOrNode& node) override;
    void visit(CAst::BitwiseLeftShiftNode& node) override;
    void visit(CAst::BitwiseRightShiftNode& node) override;
    void visit(CAst::EqualNode& node) override;
    void visit(CAst::NotEqualNode& node) override;
    void visit(CAst::GreaterNode& node) override;
    void visit(CAst::GreaterEqNode& node) override;
    void visit(CAst::LessNode& node) override;
    void visit(CAst::LessEqNode& node) override;
    void visit(CAst::IntegerValueNode& node) override;
    void visit(CAst::ModNode& node) override;
    void visit(CAst::DivNode& node) override;
    void visit(CAst::MultNode& node) override;
    void visit(CAst::PlusNode& node) override;
    void visit(CAst::MinusNode& node) override;
    void visit(CAst::ProgramNode& node) override;
    void visit(CAst::VariableDeclarationNode& node) override;
    void visit(CAst::AssignmentNode& node) override;
    void visit(CAst::NullNode& node) override;
    void visit(CAst::VariableNode& node) override;
    void visit(CAst::IfNode& node) override;
    void visit(CAst::TernaryNode& node) override;
    void visit(CAst::ForNode& node) override;
    void visit(CAst::WhileNode& node) override;
    void visit(CAst::DoWhileNode& node) override;
    void visit(CAst::ContinueNode& node) override;
    void visit(CAst::BreakNode& node) override;
    void visit(CAst::FunctionCallNode& node) override;
    template<std::derived_from<CAst::BinaryExpressionNode> T>
    void visit_binexp(CAst::BinaryExpressionNode& node);
    template<std::derived_from<CAst::UnaryExpressionNode> T>
    void visit_unexp(CAst::UnaryExpressionNode& node);
    std::shared_ptr<CAst::ProgramNode> process(CAst::ProgramNode& node);
    std::optional<SymbolEntry> lookup_symbol_current_scope(std::string name);
    std::optional<SymbolEntry> lookup_symbol(std::string name);
    void insert_symbol(std::string key, SymbolEntry value);
protected:
    std::vector<std::unordered_map<std::string, SymbolEntry>> symbol_table_;
    std::vector<std::shared_ptr<CAst::ASTNode>> buffer_;
    bool is_on_file_scope;
};

class VariableResolutionVisitor : public SemanticCAstRewriter{
public:
    ~VariableResolutionVisitor() = default;
    VariableResolutionVisitor() : SemanticCAstRewriter(), var_counter_(0), is_within_function_(false) {}
    void visit(CAst::VariableNode& node) override;
    void visit(CAst::VariableDeclarationNode& node) override;
    void visit(CAst::FunctionNode& node) override;
    void visit(CAst::FunctionCallNode& node) override;
    void new_decl(std::string name, CAst::Type type);
    std::shared_ptr<CAst::ProgramNode> process(CAst::ProgramNode& node);
private:
    int var_counter_;
    bool is_within_function_ = false;
    std::string generate_unique_var_name();
};


} // namespace Semantic
#endif // _SEMANTIC_H