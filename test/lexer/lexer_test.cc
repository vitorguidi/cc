#include "gtest/gtest.h"
#include "src/lexer/lexer.h"
#include <variant>

// Demonstrate some basic assertions.
TEST(LexerTest, BasicProgram) {
    const std::string basic_program = 
        "int main() {"
        "   return 2;"
        "}";
    Lexer l = Lexer(basic_program);
    std::vector<Token> expected_results = {
        Token{INTEGER_TYPE, std::variant{std::monostate}},
        Token{TEXT, std::variant{std::string("main")}},
        Token{LBRACE, std::variant{std::monostate}},
        Token{RETURN, std::variant{std::monostate}},
        Token{INTEGER_VALUE, std::variant{2}},
        Token{SEMICOLON, std::variant{std::monostate}},
        Token{RBRACE, std::variant{std::monostate}},
    };
}