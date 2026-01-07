#include "src/parser/parser.h"
#include "src/lexer/token.h"
#include "src/ast/c.h"
#include <stdexcept>
#include <cassert>

/*
Moving to precedence climbing
Program: [Function]*
Function: Type NAME '(' FunctionArguments ')' Block
Block: { [declaration | statement ;]* }
statement: RETURN EXPR | EXPR | IF (EXPR) STATEMENT [ELSE STATEMENT] | NULL
Type: 'int'
FunctionArguments: Type NAME [, Type NAME]*
EXPR: FACTOR | UNOP EXPR | EXPR BINOP EXPR | exp ? exp : exp
FACTOR: INTEGER_VALUE | VAR | UNOP FACTOR | (EXPR)
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
    auto body_opt = parseBlock();
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

auto RecursiveDescentParser::parseBlock() -> std::optional<std::shared_ptr<CAst::BlockNode>> {
    if (tokens_.peek(0).kind != Lexer::TokenType::LBRACE) {
        throw std::runtime_error("Expected LBRACE while parsing block.");
    }
    tokens_.consume();
    CAst::BlockNode result;
    while (tokens_.peek(0).kind != Lexer::TokenType::RBRACE) {
        if (tokens_.peek(0).kind == Lexer::TokenType::END_OF_FILE) {
            throw std::runtime_error("END_OF_FILE found during block parsing, crashing to avoid infinite loop");
        }
        if (tokens_.peek(0).kind == Lexer::TokenType::INTEGER_TYPE) {
            std::optional<std::shared_ptr<CAst::DeclarationNode>> decl = parseDeclaration();
            if(!decl) {throw std::runtime_error("Expected to parse statement within block.");}
            result.statements_.push_back(decl.value());
        } else {
            std::optional<std::shared_ptr<CAst::StatementNode>> statement = parseStatement();
            if(!statement) {throw std::runtime_error("Expected to parse statement within block.");}
            result.statements_.push_back(statement.value());
        }

    }
    if (tokens_.peek(0).kind != Lexer::TokenType::RBRACE) {
        throw std::runtime_error("Expected RBRACE at end of block");
    }
    tokens_.consume();
    return std::make_optional(std::make_shared<CAst::BlockNode>(std::move(result)));
}

auto RecursiveDescentParser::parseDeclaration() -> std::optional<std::shared_ptr<CAst::DeclarationNode>> {
    auto type = std::make_shared<CAst::TypeNode>(CAst::Type::INTEGER);
    tokens_.consume(); //get rid of type
    if (tokens_.peek(0).kind != Lexer::TokenType::NAME) {
        throw std::runtime_error("Expected NAME token after type in variable declaration");
    }
    auto variable_name = tokens_.consume();

    std::optional<std::shared_ptr<CAst::ExpressionNode>> expr = std::nullopt;

    if (tokens_.peek(0).kind != Lexer::TokenType::SEMICOLON &&
        tokens_.peek(0).kind != Lexer::TokenType::ASSIGNMENT) {
        throw std::runtime_error("Expected either SEMICOLON after declaration, or a following ASSIGNMENT.");
    }

    if (tokens_.peek(0).kind == Lexer::TokenType::SEMICOLON) {
        tokens_.consume();
        return std::make_optional(std::make_shared<CAst::DeclarationNode>(
            std::make_shared<CAst::VariableNode>(std::get<std::string>(variable_name.value)),
            type,
            expr
        ));
    }

    if (tokens_.peek(0).kind != Lexer::TokenType::ASSIGNMENT) {
        throw std::runtime_error("Expected assignment after declaration.");
    }

    tokens_.consume();

    expr = parseExpression(0);

    if (!expr) {
        throw std::runtime_error("Expected expression after assignment in declaration.");
    }

    if (tokens_.peek(0).kind != Lexer::TokenType::SEMICOLON) {
        throw std::runtime_error("Expected SEMICOLON after expression in assignment");
    }

    tokens_.consume();
    return std::make_optional(std::make_shared<CAst::DeclarationNode>(
        std::make_shared<CAst::VariableNode>(std::get<std::string>(variable_name.value)),
        type,
        expr.value()
    ));
}

auto RecursiveDescentParser::parseStatement() -> std::optional<std::shared_ptr<CAst::StatementNode>> {
    if (tokens_.peek(0).kind == Lexer::TokenType::RETURN) {
        return parseReturnStatement();
    }
    if (tokens_.peek(0).kind == Lexer::TokenType::SEMICOLON) {
        tokens_.consume();
        return std::make_shared<CAst::NullNode>();
    }
    if (tokens_.peek(0).kind == Lexer::TokenType::IF) {
        return parseIfStatement();
    }
    auto expr = parseExpression(0);
    if (!expr) {
        throw std::runtime_error("Expected expression as statement result");
    }
    if (tokens_.peek(0).kind != Lexer::TokenType::SEMICOLON) {
        throw std::runtime_error("Expected SEMICOLON after expression statement");
    }
    tokens_.consume();
    return expr;
}

auto RecursiveDescentParser::parseIfStatement() -> std::optional<std::shared_ptr<CAst::IfNode>> {
    tokens_.consume();
    if (tokens_.peek(0).kind != Lexer::TokenType::LPAREN) {
        throw std::runtime_error("Expected LPAREN before conditional expression");
    }
    tokens_.consume();
    auto expr = parseExpression(0);
    if (!expr.has_value()) {
        throw std::runtime_error("Expected conditional expression");
    }
    if (tokens_.peek(0).kind != Lexer::TokenType::RPAREN) {
        throw std::runtime_error("Expected RPAREN before conditional expression");
    }
    tokens_.consume();
    auto then = parseStatement();
    if (!then) {
        throw std::runtime_error("Expected statement after conditional expression");
    }
    if (tokens_.peek(0).kind != Lexer::TokenType::ELSE) {
        return std::make_optional(
            std::make_shared<CAst::IfNode>(
                expr.value(),
                then.value()
            )
        );
    }
    tokens_.consume();
    auto alt = parseStatement();
    return std::make_optional(
        std::make_shared<CAst::IfNode>(
            expr.value(),
            then.value(),
            alt.value()
        )
    );
}

auto RecursiveDescentParser::parseReturnStatement() -> std::optional<std::shared_ptr<CAst::ReturnStatementNode>> {
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
        case Lexer::TokenType::NAME:
            return std::make_shared<CAst::VariableNode>(
                std::get<std::string>(tokens_.consume().value)
            );
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
        if (tokens_.peek(0).kind == Lexer::TokenType::ASSIGNMENT) {
            tokens_.consume();
            auto right = parseExpression(next_token_precedence.value());
            left = std::make_shared<CAst::AssignmentNode>(left.value(), right.value());
        } else if(tokens_.peek(0).kind == Lexer::TokenType::QUESTION_MARK) {
            tokens_.consume();
            auto middle = parseExpression(0);
            if (tokens_.peek(0).kind != Lexer::TokenType::COLON) {
                throw std::runtime_error("Expected COLON after then expression in TERNARY operator");
            }
            tokens_.consume();
            auto right = parseExpression(next_token_precedence.value());
            left = std::make_optional(
                std::make_shared<CAst::TernaryNode>(
                    left.value(), middle.value(), right.value()
            ));
        } else {
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
                case Lexer::TokenType::EQUAL:
                    left = std::make_optional(std::make_shared<CAst::EqualNode>(left.value(), right.value()));
                    break;
                case Lexer::TokenType::NOT_EQUAL:
                    left = std::make_optional(std::make_shared<CAst::NotEqualNode>(left.value(), right.value()));
                    break;
                case Lexer::TokenType::GREATER:
                    left = std::make_optional(std::make_shared<CAst::GreaterNode>(left.value(), right.value()));
                    break;
                case Lexer::TokenType::GREATER_EQ:
                    left = std::make_optional(std::make_shared<CAst::GreaterEqNode>(left.value(), right.value()));
                    break;
                case Lexer::TokenType::LESS:
                    left = std::make_optional(std::make_shared<CAst::LessNode>(left.value(), right.value()));
                    break;
                case Lexer::TokenType::LESS_EQ:
                    left = std::make_optional(std::make_shared<CAst::LessEqNode>(left.value(), right.value()));
                    break;
                default:
                    throw std::runtime_error("Unable to build bin exp from given token.");
            }
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
            return std::make_optional(std::make_shared<CAst::BitwiseNotUnaryExpressionNode>(operand.value()));
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