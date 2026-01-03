#pragma once
#include <variant>
#include <string>
#include <generator>
#include <ranges>
#include <unordered_map>
#include <memory>

namespace Lexer {

typedef std::variant<std::monostate, int, std::string> TokenValue;

enum TokenType {
    INTEGER_VALUE,
    INTEGER_TYPE,
    STRING,
    NAME,
    RETURN,
    LBRACE,
    RBRACE,
    LPAREN,
    RPAREN,
    SEMICOLON,
    COMMA,
    TILDE,
    MINUS,
    DOUBLE_MINUS,
    PLUS,
    MULT,
    DIV,
    MOD,
    NOT,
    AND,
    OR,
    BITSHIFT_LEFT,
    BITSHIFT_RIGHT,
    BITWISE_AND,
    BITWISE_OR,
    BITWISE_XOR,
    END_OF_FILE,
};

struct Token {
    TokenType kind;
    TokenValue value;
};

class TokenStream {
public:
    explicit TokenStream(std::generator<Token> tokens);
    Token consume();
    Token peek(int pos_ahead);
private:
    std::generator<Token> tokens_;
    std::unordered_map<int, Token> buffer_;
    int idx_at_, idx_buffered_;
    std::unique_ptr<std::ranges::iterator_t<std::generator<Token>>> tokens_it_;
};

} //namespace Lexer