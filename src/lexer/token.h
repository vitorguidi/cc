#include <variant>

typedef TokenValue std::variant<std::monostate, int, std::string>

enum TokenType {
    INTEGER_VALUE,
    INTEGER_TYPE,
    TEXT,
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

int main() {
    return 2;
}
