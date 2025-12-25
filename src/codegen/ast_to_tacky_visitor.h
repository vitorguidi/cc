#include  "src/tacky/tacky.h"
#include "src/ast/ast.h"
#include <stdexcept>

namespace Codegen {

class AstToTackyVisitor : public CAst::Visitor {
public:
    ~AstToTackyVisitor() = default;
    void visit(CAst::TypeNode& node);
    void visit(CAst::FunctionArgumentsNode& node);
    void visit(CAst::ReturnStatementNode& node);
    void visit(CAst::StatementBlockNode& node);
    void visit(CAst::FunctionNode& node);
    void visit(CAst::ProgramNode& node);
    void visit(CAst::TildeUnaryExpressionNode& node);
    void visit(CAst::MinusUnaryExpressionNode& node);
    void visit(CAst::IntegerValueNode& node);
    std::shared_ptr<Tacky::ProgramNode> get_tacky_from_c_ast(std::shared_ptr<CAst::ProgramNode> root_node);
    std::string generate_temp_var_name();
    std::vector<std::shared_ptr<Tacky::AstNode>> result_buffer_;
    template <typename T>
    std::shared_ptr<T> get_result();
    template <typename T>
    std::vector<std::shared_ptr<T>> get_results();
    int temp_var_counter_ = 0;
};

} // namespace Codegen