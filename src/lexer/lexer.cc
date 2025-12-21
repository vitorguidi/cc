#include <string>
#include <generator>
#include "src/lexer/lexer.h"
#include <set>
#include <variant>
#include <cctype>
#include <stdexcept>

ManualLexer::ManualLexer(const std::string code) : idx_(0), code_(code) {}
auto ManualLexer::tokenize() -> std::generator<Token>  {
    while (idx_ < code_.size()) {
        for(auto c : skippable_characters_) {
            if (peek(c)) {
                idx_++;
            }
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