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
    END_OF_FILE,
};

struct Token {
    TokenType kind;
    TokenValue value;
};

