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
    void visit(CAst::DivNode& node);
    void visit(CAst::MultNode& node);
    void visit(CAst::ModNode& node);
    void visit(CAst::MinusNode& node);
    void visit(CAst::PlusNode& node);
    void visit(CAst::AndNode& node);
    void visit(CAst::BitwiseAndNode& node);
    void visit(CAst::OrNode& node);
    void visit(CAst::BitwiseOrNode& node);
    void visit(CAst::BitwiseXorNode& node);
    void visit(CAst::BitwiseLeftShiftNode& node);
    void visit(CAst::BitwiseRightShiftNode& node);
    void visit(CAst::NotUnaryExpressionNode& node);

    template<std::derived_from<Tacky::BinaryOpNode> T>
    void visit_bin_exp(CAst::BinaryExpressionNode& node);

    template<std::derived_from<Tacky::UnaryNode> T>
    void visit_un_exp(CAst::UnaryExpressionNode& node);

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