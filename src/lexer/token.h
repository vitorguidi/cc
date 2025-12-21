#pragma once
#include <variant>

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
};

struct Token {
    TokenType kind;
    TokenValue value;
};

