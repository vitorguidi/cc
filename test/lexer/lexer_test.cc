#include "gtest/gtest.h"
#include "src/lexer/lexer.h"
#include <variant>
#include <ranges>

const std::string basic_program = 
    "int main() {\n"
    "   return 2;\n"
    "}\n";

const std::string another_basic_program = 
    "int negate(int val) {\n"
    "   return ~(-(--x));\n"
    "}\n";

const std::string some_binexps = 
    "int function(int a, int b) {\n"
    "   return 2*7%5 + 10 - 11;\n"
    "}\n";

const std::string some_boolean_ops =
    "int function() {\n"
    "return !2 || 3 && 4 & 2 | 5 ^ 3 >> 4 << 10 ;"
    "}\n";

const std::string some_relational_ops =
    "int function() {\n"
    "return 2 == 3 < 4 > 5 != 7 >= 8 <= 12;"
    "}\n";

namespace Lexer {

void assert_expected_lex_results(std::vector<Token>& expected_results, TokenStream& results) {
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

TEST(LexerTest, SomeRelationalOps) {
    std::unique_ptr<Lexer> l = std::make_unique<ManualLexer>(some_relational_ops);
    std::vector<Token> expected_results = {
        Token{TokenType::INTEGER_TYPE, std::monostate{}},
        Token{TokenType::NAME, std::string("function")},
        Token{TokenType::LPAREN, std::monostate{}},
        Token{TokenType::RPAREN, std::monostate{}},
        Token{TokenType::LBRACE, std::monostate{}},
        Token{TokenType::RETURN, std::monostate{}},
        Token{TokenType::INTEGER_VALUE, 2},
        Token{TokenType::EQUAL, std::monostate{}},
        Token{TokenType::INTEGER_VALUE, 3},
        Token{TokenType::LESS, std::monostate{}},
        Token{TokenType::INTEGER_VALUE, 4},
        Token{TokenType::GREATER, std::monostate{}},
        Token{TokenType::INTEGER_VALUE, 5},
        Token{TokenType::NOT_EQUAL, std::monostate{}},
        Token{TokenType::INTEGER_VALUE, 7},
        Token{TokenType::GREATER_EQ, std::monostate{}},
        Token{TokenType::INTEGER_VALUE, 8},
        Token{TokenType::LESS_EQ, std::monostate{}},
        Token{TokenType::INTEGER_VALUE, 12},
        Token{TokenType::SEMICOLON, std::monostate{}},
        Token{TokenType::RBRACE, std::monostate{}},
    };
    auto results = l->Lex();
    assert_expected_lex_results(expected_results, results);
}

TEST(LexerTest, SomeBoolOps) {
    std::unique_ptr<Lexer> l = std::make_unique<ManualLexer>(some_boolean_ops);
    std::vector<Token> expected_results = {
        Token{TokenType::INTEGER_TYPE, std::monostate{}},
        Token{TokenType::NAME, std::string("function")},
        Token{TokenType::LPAREN, std::monostate{}},
        Token{TokenType::RPAREN, std::monostate{}},
        Token{TokenType::LBRACE, std::monostate{}},
        Token{TokenType::RETURN, std::monostate{}},
        Token{TokenType::NOT, std::monostate{}},
        Token{TokenType::INTEGER_VALUE, 2},
        Token{TokenType::OR, std::monostate{}},
        Token{TokenType::INTEGER_VALUE, 3},
        Token{TokenType::AND, std::monostate{}},
        Token{TokenType::INTEGER_VALUE, 4},
        Token{TokenType::BITWISE_AND, std::monostate{}},
        Token{TokenType::INTEGER_VALUE, 2},
        Token{TokenType::BITWISE_OR, std::monostate{}},
        Token{TokenType::INTEGER_VALUE, 5},
        Token{TokenType::BITWISE_XOR, std::monostate{}},
        Token{TokenType::INTEGER_VALUE, 3},
        Token{TokenType::BITSHIFT_RIGHT, std::monostate{}},
        Token{TokenType::INTEGER_VALUE, 4},
        Token{TokenType::BITSHIFT_LEFT, std::monostate{}},
        Token{TokenType::INTEGER_VALUE, 10},
        Token{TokenType::SEMICOLON, std::monostate{}},
        Token{TokenType::RBRACE, std::monostate{}},
    };
    auto results = l->Lex();
    assert_expected_lex_results(expected_results, results);
}

TEST(LexerTest, SomeBinexps) {
    std::unique_ptr<Lexer> l = std::make_unique<ManualLexer>(some_binexps);
    std::vector<Token> expected_results = {
        Token{TokenType::INTEGER_TYPE, std::monostate{}},
        Token{TokenType::NAME, std::string("function")},
        Token{TokenType::LPAREN, std::monostate{}},
        Token{TokenType::INTEGER_TYPE, std::monostate{}},
        Token{TokenType::NAME, std::string("a")},
        Token{TokenType::COMMA, std::monostate{}},
        Token{TokenType::INTEGER_TYPE, std::monostate{}},
        Token{TokenType::NAME, std::string("b")},
        Token{TokenType::RPAREN, std::monostate{}},
        Token{TokenType::LBRACE, std::monostate{}},
        Token{TokenType::RETURN, std::monostate{}},
        Token{TokenType::INTEGER_VALUE, 2},
        Token{TokenType::MULT, std::monostate{}},
        Token{TokenType::INTEGER_VALUE, 7},
        Token{TokenType::MOD, std::monostate{}},
        Token{TokenType::INTEGER_VALUE, 5},
        Token{TokenType::PLUS, std::monostate{}},
        Token{TokenType::INTEGER_VALUE, 10},
        Token{TokenType::MINUS, std::monostate{}},
        Token{TokenType::INTEGER_VALUE, 11},
        Token{TokenType::SEMICOLON, std::monostate{}},
        Token{TokenType::RBRACE, std::monostate{}},
    };
    auto results = l->Lex();
    assert_expected_lex_results(expected_results, results);
}

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
    auto results = l->Lex();
    assert_expected_lex_results(expected_results, results);
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
    assert_expected_lex_results(expected_results, results);
}

} // namespace Lexer

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}