#include "src/parser/parser.h"
#include "src/lexer/token.h"
#include "src/ast/ast.h"
#include <stdexcept>
#include <cassert>

/*
Language so far:

Program: [Function]*
Function: Type NAME '(' FunctionArguments ')' StatementBlock
Type: 'int'
FunctionArguments: Type NAME [, Type NAME]*
StatementBlock: '{' [Statement]* '}'
Statement: 'return' Expression ';'
Expression: INTEGER_VALUE | Unary Expression | (Expression)
Unary: ('~' | '-')
*/

namespace Parser {

RecursiveDescentParser::RecursiveDescentParser(Lexer::TokenStream tokens) : tokens_(std::move(tokens)) {}

auto RecursiveDescentParser::parse() -> std::optional<std::shared_ptr<CAst::ProgramNode>> {
    return parseProgram();
}

auto RecursiveDescentParser::parseProgram() -> std::optional<std::shared_ptr<CAst::ProgramNode>> {
    CAst::ProgramNode program;
    while (tokens_.peek(0).kind != Lexer::TokenType::END_OF_FILE) {
        auto func = parseFunction();
        if (!func.has_value()) {
            break;
        }
        program.functions_.push_back(std::move(func.value()));
    }
    return std::make_shared<CAst::ProgramNode>(program);
}

auto RecursiveDescentParser::parseFunction() -> std::optional<std::shared_ptr<CAst::FunctionNode>> {
    auto return_type_opt = parseType();
    if (!return_type_opt.has_value()) {
        return std::nullopt;
    }
    auto function_name = tokens_.consume();
    if (function_name.kind != Lexer::TokenType::NAME) {
        throw std::runtime_error("Expected function name");
    }
    auto arguments_opt = parseFunctionArguments();
    if (!arguments_opt.has_value()) {
        throw std::runtime_error("Expected function arguments");
    }
    auto body_opt = parseStatementBlock();
    if (!body_opt.has_value()) {
        throw std::runtime_error("Expected function body");
    }
    return std::make_shared<CAst::FunctionNode>(
        std::get<std::string>(function_name.value),
        return_type_opt.value(),
        arguments_opt.value(),
        body_opt.value()
    );
}

auto RecursiveDescentParser::parseType() -> std::optional<std::shared_ptr<CAst::TypeNode>> {
    switch (tokens_.peek(0).kind) {
        case Lexer::TokenType::INTEGER_TYPE: {
            auto t = tokens_.consume();
            assert(t.kind == Lexer::TokenType::INTEGER_TYPE);
            return std::make_optional(std::make_shared<CAst::TypeNode>(CAst::Type::INTEGER));
        }
        default:
            return std::nullopt;
    }
    return std::nullopt;
}

auto RecursiveDescentParser::parseFunctionArguments() -> std::optional<std::shared_ptr<CAst::FunctionArgumentsNode>> {
    auto lparen = tokens_.peek(0);
    if (lparen.kind != Lexer::TokenType::LPAREN) {
        return std::nullopt;
    }
    tokens_.consume();
    CAst::FunctionArgumentsNode function_args;
    while(tokens_.peek(0).kind != Lexer::TokenType::RPAREN) {
        auto type_node = parseType();
        if (!type_node.has_value()) {
            throw std::runtime_error("Expected argument type");
        }
        auto name_token = tokens_.consume();
        if (name_token.kind != Lexer::TokenType::NAME) {
            throw std::runtime_error("Expected argument name");
        }
        function_args.arguments_.push_back(CAst::FunctionArgument{
            .type = type_node.value()->type_,
            .name = std::get<std::string>(name_token.value),
        });
    }
    if (tokens_.peek(0).kind != Lexer::TokenType::RPAREN) {
        throw std::runtime_error("Expected closing parenthesis for function arguments");
    }
    tokens_.consume();
    return std::make_optional(std::make_shared<CAst::FunctionArgumentsNode>(std::move(function_args)));
}

auto RecursiveDescentParser::parseStatementBlock() -> std::optional<std::shared_ptr<CAst::StatementBlockNode>> {
    if (tokens_.peek(0).kind != Lexer::TokenType::LBRACE) {
        return std::nullopt;
    }
    tokens_.consume();
    CAst::StatementBlockNode result;
    while (true) {
        std::optional<std::shared_ptr<CAst::StatementNode>> statement = parseStatement();
        if (!statement.has_value()) {
            break;
        }
        result.statements_.push_back(statement.value());
    }
    return std::make_optional(std::make_shared<CAst::StatementBlockNode>(std::move(result)));
}

auto RecursiveDescentParser::parseStatement() -> std::optional<std::shared_ptr<CAst::StatementNode>> {
    if (tokens_.peek(0).kind != Lexer::TokenType::RETURN) {
        return std::nullopt;
    }
    tokens_.consume();
    auto expr = parseExpression();
    if (!expr.has_value()) {
        throw std::runtime_error("Expected return expression");
    }
    if (tokens_.peek(0).kind != Lexer::TokenType::SEMICOLON) {
        throw std::runtime_error("Expected semicolon after return statement");
    }
    tokens_.consume();
    return std::make_optional(std::make_shared<CAst::ReturnStatementNode>(
        CAst::Type::INTEGER,
        expr.value()
    ));
}

auto RecursiveDescentParser::parseExpression() -> std::optional<std::shared_ptr<CAst::ExpressionNode>> {
    if (tokens_.peek(0).kind == Lexer::TokenType::TILDE || 
        tokens_.peek(0).kind == Lexer::TokenType::MINUS) {
        return parseUnaryExpression();
    }
    if (tokens_.peek(0).kind == Lexer::TokenType::LPAREN) {
        tokens_.consume(); // consume '('
        auto expr = parseExpression();
        if (!expr.has_value()) {
            throw std::runtime_error("Expected expression after '('");
        }
        if (tokens_.peek(0).kind != Lexer::TokenType::RPAREN) {
            throw std::runtime_error("Expected ')' after expression");
        }
        tokens_.consume(); // consume ')'
        return expr;
    }
    auto constant_value = parseConstantValue();
    if (constant_value.has_value()) {
        return constant_value;
    }
    return std::nullopt;
}

auto RecursiveDescentParser::parseUnaryExpression() -> std::optional<std::shared_ptr<CAst::UnaryExpressionNode>> {
    if (tokens_.peek(0).kind == Lexer::TokenType::TILDE) {
        tokens_.consume();
        auto operand = parseExpression();
        if (!operand.has_value()) {
            throw std::runtime_error("Expected operand for unary ~");
        }
        return std::make_optional(std::make_shared<CAst::TildeUnaryExpressionNode>(operand.value()));
    }
    if (tokens_.peek(0).kind == Lexer::TokenType::MINUS) {
        tokens_.consume();
        auto operand = parseExpression();
        if (!operand.has_value()) {
            throw std::runtime_error("Expected operand for unary -");
        }
        return std::make_optional(std::make_shared<CAst::MinusUnaryExpressionNode>(operand.value()));
    }
    return std::nullopt;
}

auto RecursiveDescentParser::parseConstantValue() -> std::optional<std::shared_ptr<CAst::ConstantValueNode>> {
    if (tokens_.peek(0).kind == Lexer::TokenType::INTEGER_VALUE) {
        auto token = tokens_.consume();
        int value = std::get<int>(token.value);
        return std::make_optional(std::make_shared<CAst::IntegerValueNode>(value));
    }
    return std::nullopt;
}

} // namespace Parser;