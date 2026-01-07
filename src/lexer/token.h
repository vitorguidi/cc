#pragma once
#include <variant>
#include <string>
#include <generator>
#include <ranges>
#include <unordered_map>
#include <memory>

namespace Lexer {

typedef std::variant<std::monostate, int, std::string> TokenValue;

#define TOKEN_LIST \
    X(INTEGER_VALUE) \
    X(INTEGER_TYPE)  \
    X(STRING)        \
    X(NAME)          \
    X(RETURN)        \
    X(LBRACE)        \
    X(RBRACE)        \
    X(LPAREN)        \
    X(RPAREN)        \
    X(SEMICOLON)     \
    X(COMMA)         \
    X(TILDE)         \
    X(MINUS)         \
    X(DOUBLE_MINUS)  \
    X(PLUS)          \
    X(MULT)          \
    X(DIV)           \
    X(MOD)           \
    X(NOT)           \
    X(AND)           \
    X(OR)            \
    X(BITSHIFT_LEFT) \
    X(BITSHIFT_RIGHT)\
    X(BITWISE_AND)   \
    X(BITWISE_OR)    \
    X(BITWISE_XOR)   \
    X(EQUAL)         \
    X(NOT_EQUAL)     \
    X(LESS)          \
    X(GREATER)       \
    X(LESS_EQ)       \
    X(GREATER_EQ)    \
    X(ASSIGNMENT)    \
    X(IF)            \
    X(ELSE)          \
    X(QUESTION_MARK) \
    X(COLON)         \
    X(DO)            \
    X(WHILE)         \
    X(FOR)           \
    X(BREAK)         \
    X(CONTINUE)      \
    X(END_OF_FILE)

enum TokenType {
#define X(name) name,
    TOKEN_LIST
#undef X
};

inline std::string token_type_to_string(TokenType type) {
    switch (type) {
#define X(name) case name: return #name;
        TOKEN_LIST
#undef X
        default: return "UNKNOWN";
    }
}

// enum TokenType {
//     INTEGER_VALUE,
//     INTEGER_TYPE,
//     STRING,
//     NAME,
//     RETURN,
//     LBRACE,
//     RBRACE,
//     LPAREN,
//     RPAREN,
//     SEMICOLON,
//     COMMA,
//     TILDE,
//     MINUS,
//     DOUBLE_MINUS,
//     PLUS,
//     MULT,
//     DIV,
//     MOD,
//     NOT,
//     AND,
//     OR,
//     BITSHIFT_LEFT,
//     BITSHIFT_RIGHT,
//     BITWISE_AND,
//     BITWISE_OR,
//     BITWISE_XOR,
//     EQUAL,
//     NOT_EQUAL,
//     LESS,
//     GREATER,
//     LESS_EQ,
//     GREATER_EQ,
//     ASSIGNMENT,
//     IF,
//     ELSE,
//     QUESTION_MARK,
//     COLON,
//     DO,
//     WHILE,
//     FOR,
//     BREAK,
//     CONTINUE,
//     END_OF_FILE,
// };

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