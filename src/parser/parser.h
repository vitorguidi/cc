#include "src/ast/ast.h"
#include <generator>
#include "src/lexer/token.h"
#include <optional>

namespace Parser {

class Parser {
public:
    virtual auto parse() -> std::optional<std::shared_ptr<CAst::ProgramNode>> = 0;
    ~Parser() = default;
};

class RecursiveDescentParser : public Parser {
public:
    RecursiveDescentParser(Lexer::TokenStream tokens);
    auto parse() -> std::optional<std::shared_ptr<CAst::ProgramNode>> override;
    ~RecursiveDescentParser() = default;
private:
    Lexer::TokenStream tokens_;
    std::optional<std::shared_ptr<CAst::ProgramNode>> parseProgram();
    std::optional<std::shared_ptr<CAst::FunctionNode>> parseFunction();
    std::optional<std::shared_ptr<CAst::TypeNode>> parseType();
    std::optional<std::shared_ptr<CAst::FunctionArgumentsNode>> parseFunctionArguments();
    std::optional<std::shared_ptr<CAst::StatementBlockNode>> parseStatementBlock();
    std::optional<std::shared_ptr<CAst::StatementNode>> parseStatement();
    std::optional<std::shared_ptr<CAst::ExpressionNode>> parseExpression();
    std::optional<std::shared_ptr<CAst::UnaryExpressionNode>> parseUnaryExpression();
    std::optional<std::shared_ptr<CAst::ConstantValueNode>> parseConstantValue();
};

} // namespace Parser