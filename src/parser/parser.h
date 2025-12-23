#include "src/ast/ast.h"
#include <generator>
#include "src/lexer/token.h"
#include <optional>

class Parser {
public:
    virtual auto parse() -> std::optional<std::shared_ptr<ProgramNode>> = 0;
    ~Parser() = default;
};

class RecursiveDescentParser : public Parser {
public:
    RecursiveDescentParser(TokenStream tokens);
    auto parse() -> std::optional<std::shared_ptr<ProgramNode>> override;
    ~RecursiveDescentParser() = default;
private:
    TokenStream tokens_;
    std::optional<std::shared_ptr<ProgramNode>> parseProgram();
    std::optional<std::shared_ptr<FunctionNode>> parseFunction();
    std::optional<std::shared_ptr<TypeNode>> parseType();
    std::optional<std::shared_ptr<FunctionArgumentsNode>> parseFunctionArguments();
    std::optional<std::shared_ptr<StatementBlockNode>> parseStatementBlock();
    std::optional<std::shared_ptr<StatementNode>> parseStatement();
    std::optional<std::shared_ptr<ExpressionNode>> parseExpression();
    std::optional<std::shared_ptr<UnaryExpressionNode>> parseUnaryExpression();
    std::optional<std::shared_ptr<ConstantValueNode>> parseConstantValue();
};