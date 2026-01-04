#include <string>
#include <generator>
#include "src/lexer/lexer.h"
#include <set>
#include <variant>
#include <cctype>
#include <stdexcept>
#include <memory>
#include <algorithm>

namespace Lexer {

TokenStream::TokenStream(std::generator<Token> tokens) : 
    tokens_(std::move(tokens)),
    buffer_(std::unordered_map<int, Token>()),
    idx_at_(0),
    idx_buffered_(-1),
    tokens_it_(nullptr) {
        tokens_it_ = std::make_unique<std::ranges::iterator_t<std::generator<Token>>>(tokens_.begin());
}
Token TokenStream::consume() {
    if (buffer_.empty()) {
        if (*tokens_it_ == tokens_.end()) {
            return Token{TokenType::END_OF_FILE, std::monostate{}};
        }
        buffer_[++idx_buffered_] = **tokens_it_;
        (*tokens_it_)++;
    }
    Token t = buffer_[idx_at_];
    buffer_.erase(idx_at_);
    idx_at_++;
    return t;
}

Token TokenStream::peek(int pos_ahead) {
    while (idx_buffered_ < idx_at_ + pos_ahead) {
        if (*tokens_it_ == tokens_.end()) {
            return Token{TokenType::END_OF_FILE, std::monostate{}};
        }
        buffer_[++idx_buffered_] = **tokens_it_;
        (*tokens_it_)++;
    }
    return buffer_[idx_at_ + pos_ahead];
}

ManualLexer::ManualLexer(const std::string code) : idx_(0), code_(code) {}
auto ManualLexer::tokenize() -> std::generator<Token>  {
    while (idx_ < code_.size()) {
        while (skip()) {
            idx_++;
        }
        if (idx_ >= code_.size()) {
            break;
        }
        if (peek('(')) {
            co_yield Token{TokenType::LPAREN, std::monostate{}};
            idx_++;
            continue;
        }
        if (peek(')')) {
            co_yield Token{TokenType::RPAREN, std::monostate{}};
            idx_++;
            continue;
        }
        if (peek('{')) {
            co_yield Token{TokenType::LBRACE, std::monostate{}};
            idx_++;
            continue;
        }
        if (peek('}')) {
            co_yield Token{TokenType::RBRACE, std::monostate{}};
            idx_++;
            continue;
        }
        if (peek(';')) {
            co_yield Token{TokenType::SEMICOLON, std::monostate{}};
            idx_++;
            continue;
        }
        if (peek(',')) {
            co_yield Token{TokenType::COMMA, std::monostate{}};
            idx_++;
            continue;
        }
        if (peek('~')) {
            co_yield Token{TokenType::TILDE, std::monostate{}};
            idx_++;
            continue;
        }
        if (peek("--")) {
            co_yield Token{TokenType::DOUBLE_MINUS, std::monostate{}};
            idx_+=2;
            continue;
        }
        if (peek('-')) {
            co_yield Token{TokenType::MINUS, std::monostate{}};
            idx_++;
            continue;
        }
        if (peek('+')) {
            co_yield Token{TokenType::PLUS, std::monostate{}};
            idx_++;
            continue;
        }
        if (peek('*')) {
            co_yield Token{TokenType::MULT, std::monostate{}};
            idx_++;
            continue;
        }
        if (peek('/')) {
            co_yield Token{TokenType::DIV, std::monostate{}};
            idx_++;
            continue;
        }
        if (peek('%')) {
            co_yield Token{TokenType::MOD, std::monostate{}};
            idx_++;
            continue;
        }
        if (peek("&&")) {
            co_yield Token{TokenType::AND, std::monostate{}};
            idx_+=2;
            continue;
        }
        if (peek('&')) {
            co_yield Token{TokenType::BITWISE_AND, std::monostate{}};
            idx_++;
            continue;
        }
        if (peek("||")) {
            co_yield Token{TokenType::OR, std::monostate{}};
            idx_+=2;
            continue;
        }
        if (peek('|')) {
            co_yield Token{TokenType::BITWISE_OR, std::monostate{}};
            idx_++;
            continue;
        }
        if (peek("^")) {
            co_yield Token{TokenType::BITWISE_XOR, std::monostate{}};
            idx_++;
            continue;
        }
        if (peek("!=")) {
            co_yield Token{TokenType::NOT_EQUAL, std::monostate{}};
            idx_+=2;
            continue;
        }
        if (peek('!')) {
            co_yield Token{TokenType::NOT, std::monostate{}};
            idx_++;
            continue;
        }
        if (peek("<<")) {
            co_yield Token{TokenType::BITSHIFT_LEFT, std::monostate{}};
            idx_+=2;
            continue;      
        }
        if (peek("<=")) {
            co_yield Token{TokenType::LESS_EQ, std::monostate{}};
            idx_+=2;
            continue; 
        }
        if (peek(">>")) {
            co_yield Token{TokenType::BITSHIFT_RIGHT, std::monostate{}};
            idx_+=2;
            continue;      
        }
        if (peek(">=")) {
            co_yield Token{TokenType::GREATER_EQ, std::monostate{}};
            idx_+=2;
            continue; 
        }
        if (peek("<")) {
            co_yield Token{TokenType::LESS, std::monostate{}};
            idx_++;
            continue; 
        }
        if (peek(">")) {
            co_yield Token{TokenType::GREATER, std::monostate{}};
            idx_++;
            continue; 
        }
        if (peek("==")) {
            co_yield Token{TokenType::EQUAL, std::monostate{}};
            idx_+=2;
            continue; 
        }
        if (peek(std::string("int"))) {
            co_yield Token{TokenType::INTEGER_TYPE, std::monostate{}};
            idx_+=3;
            continue;
        }
        if (peek(std::string("return"))) {
            co_yield Token{TokenType::RETURN, std::monostate{}};
            idx_+=6;
            continue;
        }
        //TODO: handle int_min and int_max
        std::string value;
        if (isdigit(code_[idx_])) {
            while (idx_ < code_.size() && isdigit(code_[idx_])) {
                value.push_back(code_[idx_]);
                idx_++;
            }
            co_yield Token{TokenType::INTEGER_VALUE, std::stoi(value)};
            continue;
        }
        if (peek('"')) {
            idx_++;
            while(!peek('"')) {
                value.push_back(code_[idx_]);
            }
            idx_++;
            co_yield Token{TokenType::STRING, value};
            continue;
        }
        while (idx_ < code_.size() && isalpha(code_[idx_])) {
            value.push_back(code_[idx_]);
            idx_++;
        }
        co_yield Token{TokenType::NAME, value};
    }
    co_yield Token{TokenType::END_OF_FILE, std::monostate{}};
    co_return;
}
bool ManualLexer::skip() {
    if (idx_ >= code_.size()) {
        return false;
    }
    for (auto c : skippable_characters_) {
        if (peek(c)) {
            return true;
        }
    }
    return false;
}
bool ManualLexer::peek(std::string expected) {
    if (expected.size() + idx_ > code_.size()) {
        return false;
    }
    return code_.substr(idx_, expected.size()) == expected;
}
bool ManualLexer::peek(char c) {
    if (idx_ >= code_.size()) {
        return false;
    }
    return code_[idx_] == c;
}
auto ManualLexer::Lex() -> TokenStream {
    return TokenStream(tokenize());
}

} // namespace Lexer