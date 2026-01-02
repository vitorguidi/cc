#ifndef _PARSER_H_
#define _PARSER_H_

#include "src/ast/ast.h"
#include <generator>
#include "src/lexer/token.h"
#include <optional>
#include <unordered_map>

namespace Parser {

enum Associativity {
    LEFT,
    RIGHT
};

inline bool is_bin_op(Lexer::TokenType token_type) {
    switch(token_type) {
        case Lexer::TokenType::DIV:
        case Lexer::TokenType::MULT:
        case Lexer::TokenType::MOD:
        case Lexer::TokenType::MINUS:
        case Lexer::TokenType::PLUS:
            return true;
        default:
            return false;
    }
}

inline std::optional<Associativity> associativity(Lexer::TokenType token_type) {
    switch (token_type) {
        case Lexer::TokenType::MOD:
        case Lexer::TokenType::MULT:
        case Lexer::TokenType::DIV:
        case Lexer::TokenType::PLUS:
        case Lexer::TokenType::MINUS:
            return std::make_optional(LEFT);
        default:
            return std::nullopt;
    }
}

typedef int Precedence;

inline std::optional<Precedence> precedence(Lexer::TokenType token_type) {
    switch (token_type) {
        case Lexer::TokenType::MOD:
        case Lexer::TokenType::DIV:
        case Lexer::TokenType::MULT:
            return std::make_optional(50);
        case Lexer::TokenType::PLUS:
        case Lexer::TokenType::MINUS:
            return std::make_optional(45);
        default:
            return std::nullopt;
    }
}

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
    std::optional<std::shared_ptr<CAst::ExpressionNode>> parseExpression(int min_precedence);
    std::optional<std::shared_ptr<CAst::ExpressionNode>> parseFactor();
    std::optional<std::shared_ptr<CAst::UnaryExpressionNode>> parseUnaryExpression();
    std::optional<std::shared_ptr<CAst::ConstantValueNode>> parseConstantValue();
};

} // namespace Parser

#endif // _PARSER_H_