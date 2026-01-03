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

/*
Moving to precedence climbing
Program: [Function]*
Function: Type NAME '(' FunctionArguments ')' StatementBlock
Type: 'int'
FunctionArguments: Type NAME [, Type NAME]*
StatementBlock: '{' [Statement]* '}'
Statement: 'return' EXPR ';'
EXPR: FACTOR | UNOP EXPR | EXPR BINOP EXPR
FACTOR: INTEGER_VALUE | UNOP FACTOR | (EXPR)
BINOP: + | - | * | / | %
UNOP: ('~' | '-')
*/

namespace Parser {

//  TODO(vitorguidi): just throw exceptions instead of returning empty optional, if we parse we already
//  did the peek check and actually expect the result.

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
        if (tokens_.peek(0).kind == Lexer::TokenType::COMMA) {
            tokens_.consume();
        }
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
    auto expr = parseExpression(0);
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

auto RecursiveDescentParser::parseFactor() -> std::optional<std::shared_ptr<CAst::ExpressionNode>> {
    auto cur_token = tokens_.peek(0);
    switch (cur_token.kind) {
        case Lexer::TokenType::INTEGER_VALUE:
            return parseConstantValue();
        case Lexer::TokenType::TILDE:
        case Lexer::TokenType::NOT:
        case Lexer::TokenType::MINUS: {
            auto unop = parseUnaryExpression();
            if (!unop) {
                throw std::runtime_error("Expected unary expression during factor parsing.");
            }
            return unop;
        }
        case Lexer::TokenType::LPAREN: {
            tokens_.consume();
            auto inner_exp = parseExpression(0);
            if (!inner_exp) {
                throw std::runtime_error("Expected expression within parens while expr parsing.");
            }
            if (tokens_.peek(0).kind != Lexer::TokenType::RPAREN) {
                throw std::runtime_error("Expected RPAREN to close parenthetised expr while factor parsing.");
            }
            tokens_.consume();
            return inner_exp;
        }
        default:
            throw std::runtime_error("Failed to parse factor, malformed input.");
    }
}

auto RecursiveDescentParser::parseExpression(int min_precedence) -> std::optional<std::shared_ptr<CAst::ExpressionNode>> {
    auto left = parseFactor();
    if (!left) {
        throw std::runtime_error("Expected a left factor during expr parsing");
    }
    auto next_token = tokens_.peek(0).kind;
    auto next_token_precedence = precedence(next_token);
    while(is_bin_op(next_token) && next_token_precedence.value() >= min_precedence) {
        Lexer::Token op = tokens_.consume();
        std::optional<std::shared_ptr<CAst::ExpressionNode>> right = parseExpression(next_token_precedence.value() + 1);
        if (!right) {
            throw std::runtime_error("Expected to parse right expression during precedence climbing");
        }
        switch (next_token) {
            case Lexer::TokenType::PLUS:
                left = std::make_optional(std::make_shared<CAst::PlusNode>(left.value(), right.value()));
                break;
            case Lexer::TokenType::MINUS:
                left = std::make_optional(std::make_shared<CAst::MinusNode>(left.value(), right.value()));
                break;
            case Lexer::TokenType::MOD:
                left = std::make_optional(std::make_shared<CAst::ModNode>(left.value(), right.value()));
                break;
            case Lexer::TokenType::DIV:
                left = std::make_optional(std::make_shared<CAst::DivNode>(left.value(), right.value()));
                break;
            case Lexer::TokenType::MULT:
                left = std::make_optional(std::make_shared<CAst::MultNode>(left.value(), right.value()));
                break;
            case Lexer::TokenType::AND:
                left = std::make_optional(std::make_shared<CAst::AndNode>(left.value(), right.value()));
                break;
            case Lexer::TokenType::BITWISE_AND:
                left = std::make_optional(std::make_shared<CAst::BitwiseAndNode>(left.value(), right.value()));
                break;
            case Lexer::TokenType::OR:
                left = std::make_optional(std::make_shared<CAst::OrNode>(left.value(), right.value()));
                break;
            case Lexer::TokenType::BITWISE_OR:
                left = std::make_optional(std::make_shared<CAst::BitwiseOrNode>(left.value(), right.value()));
                break;
            case Lexer::TokenType::BITWISE_XOR:
                left = std::make_optional(std::make_shared<CAst::BitwiseXorNode>(left.value(), right.value()));
                break;
            case Lexer::TokenType::BITSHIFT_LEFT:
                left = std::make_optional(std::make_shared<CAst::BitwiseLeftShiftNode>(left.value(), right.value()));
                break;
            case Lexer::TokenType::BITSHIFT_RIGHT:
                left = std::make_optional(std::make_shared<CAst::BitwiseRightShiftNode>(left.value(), right.value()));
                break;
            default:
                throw std::runtime_error("Unable to build bin exp from given token.");
        }
        next_token = tokens_.peek(0).kind;
        next_token_precedence = precedence(next_token);
    }
    return left;
}

auto RecursiveDescentParser::parseUnaryExpression() -> std::optional<std::shared_ptr<CAst::UnaryExpressionNode>> {
    auto token = tokens_.consume();
    auto operand = parseFactor();
    if (!operand.has_value()) {
        throw std::runtime_error("Expected operand for unary op");
    }
    switch (token.kind) {
        case Lexer::TokenType::TILDE:
            return std::make_optional(std::make_shared<CAst::TildeUnaryExpressionNode>(operand.value()));
        case Lexer::TokenType::MINUS:
            return std::make_optional(std::make_shared<CAst::MinusUnaryExpressionNode>(operand.value()));
        case Lexer::TokenType::NOT:
            return std::make_optional(std::make_shared<CAst::NotUnaryExpressionNode>(operand.value()));
        default:
            throw std::runtime_error("Unexpecetd operation token while emiting a unary expr.");
    }
}

auto RecursiveDescentParser::parseConstantValue() -> std::optional<std::shared_ptr<CAst::ConstantValueNode>> {
    if (tokens_.peek(0).kind == Lexer::TokenType::INTEGER_VALUE) {
        auto token = tokens_.consume();
        int value = std::get<int>(token.value);
        return std::make_optional(std::make_shared<CAst::IntegerValueNode>(value));
    }
    throw std::runtime_error("Expected integer token while parsing constant value");
}

} // namespace Parser;