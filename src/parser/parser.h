#include "src/ast/ast.h"
#include <generator>
#include "src/lexer/token.h"
#include <optional>

class Parser {
public:
    virtual auto parse() -> std::optional<ProgramNode> = 0;
    ~Parser() = default;
};

class RecursiveDescentParser : public Parser {
public:
    RecursiveDescentParser(TokenStream tokens);
    auto parse() -> std::optional<ProgramNode> override;
    ~RecursiveDescentParser() = default;
private:
    TokenStream tokens_;
    std::optional<ProgramNode> parseProgram();
    std::optional<FunctionNode> parseFunction();
    std::optional<TypeNode> parseType();
    std::optional<FunctionArgumentsNode> parseFunctionArguments();
    std::optional<StatementBlockNode> parseStatementBlock();
    std::optional<StatementNode> parseStatement();
};