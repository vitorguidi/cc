#include "src/parser/parser.h"
#include "src/lexer/token.h"
#include "src/ast/ast.h"
#include <stdexcept>
#include <cassert>

RecursiveDescentParser::RecursiveDescentParser(TokenStream tokens) : tokens_(std::move(tokens)) {}

auto RecursiveDescentParser::parse() -> std::optional<ProgramNode> {
    return parseProgram();
}

auto RecursiveDescentParser::parseProgram() -> std::optional<ProgramNode> {
    ProgramNode program;
    while (tokens_.peek(0).kind != TokenType::END_OF_FILE) {
        auto func = parseFunction();
        if (!func.has_value()) {
            break;
        }
        program.functions_.push_back(func.value());
    }
    return program;
}

auto RecursiveDescentParser::parseFunction() -> std::optional<FunctionNode> {
    std::optional<TypeNode> return_type_opt = parseType();
    if (!return_type_opt.has_value()) {
        return std::nullopt;
    }
    auto function_name = tokens_.consume();
    if (function_name.kind != TokenType::NAME) {
        throw std::runtime_error("Expected function name");
    }
    std::optional<FunctionArgumentsNode> arguments_opt = parseFunctionArguments();
    if (!arguments_opt.has_value()) {
        throw std::runtime_error("Expected function arguments");
    }
    std::optional<StatementBlockNode> body_opt = parseStatementBlock();
    if (!body_opt.has_value()) {
        throw std::runtime_error("Expected function body");
    }
    return FunctionNode{
        .name_ = std::get<std::string>(function_name.value),
        .return_type_ = return_type_opt.value(),
        .arguments_ = arguments_opt.value(),
        .body_ = body_opt.value()
    };
}

auto RecursiveDescentParser::parseType() -> std::optional<TypeNode> {
    switch (tokens_.peek(0).kind) {
        case TokenType::INTEGER_TYPE: {
            auto t = tokens_.consume();
            assert(t.kind == TokenType::INTEGER_TYPE);
            return std::make_optional(TypeNode{ .return_type_ = Type::INTEGER });
        }
        default:
            return std::nullopt;
    }
    return std::nullopt;
}

auto RecursiveDescentParser::parseFunctionArguments() -> std::optional<FunctionArgumentsNode> {
    auto lparen = tokens_.peek(0);
    if (lparen.kind != TokenType::LPAREN) {
        return std::nullopt;
    }
    tokens_.consume();
    FunctionArgumentsNode args;
    while(tokens_.peek(0).kind != TokenType::RPAREN) {
        auto type_node = parseType();
        if (!type_node.has_value()) {
            throw std::runtime_error("Expected argument type");
        }
        auto name_token = tokens_.consume();
        if (name_token.kind != TokenType::NAME) {
            throw std::runtime_error("Expected argument name");
        }
        args.arguments_.push_back(FunctionArgument{
            .type = type_node->return_type_,
            .name = std::get<std::string>(name_token.value),
        });
    }
    if (tokens_.peek(0).kind != TokenType::RPAREN) {
        throw std::runtime_error("Expected closing parenthesis for function arguments");
    }
    tokens_.consume();
    return args;
}

auto RecursiveDescentParser::parseStatementBlock() -> std::optional<StatementBlockNode> {
    if (tokens_.peek(0).kind != TokenType::LBRACE) {
        return std::nullopt;
    }
    tokens_.consume();
    StatementBlockNode result;
    while (true) {
        std::optional<StatementNode> statement = parseStatement();
        if (!statement.has_value()) {
            break;
        }
        result.statements_.push_back(statement.value());
    }
    return std::make_optional(result);
}

auto RecursiveDescentParser::parseStatement() -> std::optional<StatementNode> {
    if (tokens_.peek(0).kind != TokenType::RETURN) {
        return std::nullopt;
    }
    tokens_.consume();
    if (tokens_.peek(0).kind != TokenType::INTEGER_VALUE) {
        throw std::runtime_error("Expected integer return value");
    }
    Token return_value_token = tokens_.consume();
    if (tokens_.peek(0).kind != TokenType::SEMICOLON) {
        throw std::runtime_error("Expected semicolon after return statement");
    }
    tokens_.consume();
    StatementNode return_stmt{
        .type_ = StatementType::RETURN_STATEMENT,
        .parameters_ = ReturnParameters{
            .return_type_ = Type::INTEGER,
            .return_value_ = std::get<int>(return_value_token.value),
        }
    };
    return std::make_optional<StatementNode>(return_stmt);
}