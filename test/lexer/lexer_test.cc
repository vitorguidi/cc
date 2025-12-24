#include "gtest/gtest.h"
#include "src/lexer/lexer.h"
#include <variant>
#include <ranges>

const std::string basic_program = 
    "int main() {"
    "   return 2;"
    "}";

const std::string another_basic_program = 
    "int negate(int val) {"
    "   return ~(-(--x));"
    "}";

namespace Lexer {

TEST(LexerTest, AnotherBasicLex) {
    std::unique_ptr<Lexer> l = std::make_unique<ManualLexer>(another_basic_program);
    std::vector<Token> expected_results = {
        Token{TokenType::INTEGER_TYPE, std::monostate{}},
        Token{TokenType::NAME, std::string("negate")},
        Token{TokenType::LPAREN,  std::monostate{}},
        Token{TokenType::INTEGER_TYPE, std::monostate{}},
        Token{TokenType::NAME, std::string("val")},
        Token{TokenType::RPAREN,  std::monostate{}},
        Token{TokenType::LBRACE, std::monostate{}},
        Token{TokenType::RETURN, std::monostate{}},
        Token{TokenType::TILDE, std::monostate{}},
        Token{TokenType::LPAREN, std::monostate{}},
        Token{TokenType::MINUS, std::monostate{}},
        Token{TokenType::LPAREN, std::monostate{}},
        Token{TokenType::DOUBLE_MINUS, std::monostate{}},
        Token{TokenType::NAME, std::string("x")},
        Token{TokenType::RPAREN, std::monostate{}},
        Token{TokenType::RPAREN, std::monostate{}},
        Token{TokenType::SEMICOLON, std::monostate{}},
        Token{TokenType::RBRACE, std::monostate{}},
        Token{TokenType::END_OF_FILE, std::monostate{}},
    };

}

TEST(LexerTest, BasicProgramLex) {
    std::unique_ptr<Lexer> l = std::make_unique<ManualLexer>(basic_program);
    std::vector<Token> expected_results = {
        Token{TokenType::INTEGER_TYPE, std::monostate{}},
        Token{TokenType::NAME, std::string("main")},
        Token{TokenType::LPAREN,  std::monostate{}},
        Token{TokenType::RPAREN,  std::monostate{}},
        Token{TokenType::LBRACE, std::monostate{}},
        Token{TokenType::RETURN, std::monostate{}},
        Token{TokenType::INTEGER_VALUE, 2},
        Token{TokenType::SEMICOLON, std::monostate{}},
        Token{TokenType::RBRACE, std::monostate{}},
        Token{TokenType::END_OF_FILE, std::monostate{}},
    };
    auto results = l->Lex();
    int idx_at = 0;
    for(auto expected_token: expected_results) {
        size_t delta = 0;
        // Peaks from current position to the end of expected results yields the actual
        // expected tokens
        for(; delta + idx_at < expected_results.size(); delta++) {
            Token peeked_token = results.peek(delta);
            Token expected_peek_token = expected_results[idx_at + delta];
            EXPECT_EQ(peeked_token.kind, expected_peek_token.kind);
            EXPECT_EQ(peeked_token.value, expected_peek_token.value);
        }
        // Ensure peeking beyond the expected results yields END_OF_FILE
        for(; idx_at + delta < 3*expected_results.size(); delta++) {
            Token peeked_token = results.peek(delta);
            EXPECT_EQ(peeked_token.kind, TokenType::END_OF_FILE);
            EXPECT_EQ(peeked_token.value, TokenValue(std::monostate{}));
        }
        // Finally, consumes return what we expect
        Token consumed_token = results.consume();
        EXPECT_EQ(consumed_token.kind, expected_token.kind);
        EXPECT_EQ(consumed_token.value, expected_token.value);
        idx_at++;
    }
}

} // namespace Lexer

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}