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

RecursiveDescentParser::RecursiveDescentParser(TokenStream tokens) : tokens_(std::move(tokens)) {}

auto RecursiveDescentParser::parse() -> std::optional<std::shared_ptr<ProgramNode>> {
    return parseProgram();
}

auto RecursiveDescentParser::parseProgram() -> std::optional<std::shared_ptr<ProgramNode>> {
    ProgramNode program;
    while (tokens_.peek(0).kind != TokenType::END_OF_FILE) {
        auto func = parseFunction();
        if (!func.has_value()) {
            break;
        }
        program.functions_.push_back(std::move(func.value()));
    }
    return std::make_shared<ProgramNode>(program);
}

auto RecursiveDescentParser::parseFunction() -> std::optional<std::shared_ptr<FunctionNode>> {
    auto return_type_opt = parseType();
    if (!return_type_opt.has_value()) {
        return std::nullopt;
    }
    auto function_name = tokens_.consume();
    if (function_name.kind != TokenType::NAME) {
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
    return std::make_shared<FunctionNode>(
        std::get<std::string>(function_name.value),
        return_type_opt.value(),
        arguments_opt.value(),
        body_opt.value()
    );
}

auto RecursiveDescentParser::parseType() -> std::optional<std::shared_ptr<TypeNode>> {
    switch (tokens_.peek(0).kind) {
        case TokenType::INTEGER_TYPE: {
            auto t = tokens_.consume();
            assert(t.kind == TokenType::INTEGER_TYPE);
            return std::make_optional(std::make_shared<TypeNode>(Type::INTEGER));
        }
        default:
            return std::nullopt;
    }
    return std::nullopt;
}

auto RecursiveDescentParser::parseFunctionArguments() -> std::optional<std::shared_ptr<FunctionArgumentsNode>> {
    auto lparen = tokens_.peek(0);
    if (lparen.kind != TokenType::LPAREN) {
        return std::nullopt;
    }
    tokens_.consume();
    FunctionArgumentsNode function_args;
    while(tokens_.peek(0).kind != TokenType::RPAREN) {
        auto type_node = parseType();
        if (!type_node.has_value()) {
            throw std::runtime_error("Expected argument type");
        }
        auto name_token = tokens_.consume();
        if (name_token.kind != TokenType::NAME) {
            throw std::runtime_error("Expected argument name");
        }
        function_args.arguments_.push_back(FunctionArgument{
            .type = type_node.value()->type_,
            .name = std::get<std::string>(name_token.value),
        });
    }
    if (tokens_.peek(0).kind != TokenType::RPAREN) {
        throw std::runtime_error("Expected closing parenthesis for function arguments");
    }
    tokens_.consume();
    return std::make_optional(std::make_shared<FunctionArgumentsNode>(std::move(function_args)));
}

auto RecursiveDescentParser::parseStatementBlock() -> std::optional<std::shared_ptr<StatementBlockNode>> {
    if (tokens_.peek(0).kind != TokenType::LBRACE) {
        return std::nullopt;
    }
    tokens_.consume();
    StatementBlockNode result;
    while (true) {
        std::optional<std::shared_ptr<StatementNode>> statement = parseStatement();
        if (!statement.has_value()) {
            break;
        }
        result.statements_.push_back(statement.value());
    }
    return std::make_optional(std::make_shared<StatementBlockNode>(std::move(result)));
}

auto RecursiveDescentParser::parseStatement() -> std::optional<std::shared_ptr<StatementNode>> {
    if (tokens_.peek(0).kind != TokenType::RETURN) {
        return std::nullopt;
    }
    tokens_.consume();
    auto expr = parseExpression();
    if (!expr.has_value()) {
        throw std::runtime_error("Expected return expression");
    }
    if (tokens_.peek(0).kind != TokenType::SEMICOLON) {
        throw std::runtime_error("Expected semicolon after return statement");
    }
    tokens_.consume();
    return std::make_optional(std::make_shared<ReturnStatementNode>(
        Type::INTEGER,
        expr.value()
    ));
}

auto RecursiveDescentParser::parseExpression() -> std::optional<std::shared_ptr<ExpressionNode>> {
    if (tokens_.peek(0).kind == TokenType::TILDE || 
        tokens_.peek(0).kind == TokenType::MINUS) {
        return parseUnaryExpression();
    }
    if (tokens_.peek(0).kind == TokenType::LPAREN) {
        tokens_.consume(); // consume '('
        auto expr = parseExpression();
        if (!expr.has_value()) {
            throw std::runtime_error("Expected expression after '('");
        }
        if (tokens_.peek(0).kind != TokenType::RPAREN) {
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

auto RecursiveDescentParser::parseUnaryExpression() -> std::optional<std::shared_ptr<UnaryExpressionNode>> {
    if (tokens_.peek(0).kind == TokenType::TILDE) {
        tokens_.consume();
        auto operand = parseExpression();
        if (!operand.has_value()) {
            throw std::runtime_error("Expected operand for unary ~");
        }
        return std::make_optional(std::make_shared<TildeUnaryExpressionNode>(operand.value()));
    }
    if (tokens_.peek(0).kind == TokenType::MINUS) {
        tokens_.consume();
        auto operand = parseExpression();
        if (!operand.has_value()) {
            throw std::runtime_error("Expected operand for unary -");
        }
        return std::make_optional(std::make_shared<MinusUnaryExpressionNode>(operand.value()));
    }
    return std::nullopt;
}

auto RecursiveDescentParser::parseConstantValue() -> std::optional<std::shared_ptr<ConstantValueNode>> {
    if (tokens_.peek(0).kind == TokenType::INTEGER_VALUE) {
        auto token = tokens_.consume();
        int value = std::get<int>(token.value);
        return std::make_optional(std::make_shared<IntegerValueNode>(value));
    }
    return std::nullopt;
}