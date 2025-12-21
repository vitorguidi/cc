#include "gtest/gtest.h"
#include "src/lexer/lexer.h"
#include <variant>
#include <ranges>

// Demonstrate some basic assertions.
TEST(LexerTest, BasicProgram) {
    const std::string basic_program = 
        "int main() {"
        "   return 2;"
        "}";
    std::unique_ptr<Lexer> l = std::make_unique<ManualLexer>(basic_program);
    std::vector<Token> expected_results = {
        Token{TokenType::INTEGER_TYPE, std::monostate{}},
        Token{TokenType::NAME, std::string("main")},
        Token{TokenType::LBRACE, std::monostate{}},
        Token{TokenType::RETURN, std::monostate{}},
        Token{TokenType::INTEGER_VALUE, 2},
        Token{TokenType::SEMICOLON, std::monostate{}},
        Token{TokenType::RBRACE, std::monostate{}},
    };
    auto results = l->tokenize();
    for(auto x : results) {
        auto y = std::move(x);
    }
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}