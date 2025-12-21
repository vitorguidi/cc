#include <string>
#include <generator>
#include <src/lexer/lexer.h>
#include <set>
#include <variant>
#include <cctype>
#include <stdexcept>

class ManualLexer: public Lexer {
public:
    ManualLexer(const std::string& code) : code(code_) {}
    std::generator<Token> tokenize() {
        while (idx < code_.size()) {
            for(auto c : skippable_characters_) {
                if (peek(c)) {
                    idx++;
                }
            }
            if (idx >= code_.size) {
                break;
            }
            TokenType token_type;
            std::variant
            if (peek('(')) {
                co_yield Token{TokenType::LPAREN, std::variant{std::monostate}};
                continue;
            }
            if (peek(')')) {
                co_yield Token{TokenType::RPAREN, std::variant{std::monostate}};
                continue;
            }
            if (peek('{')) {
                co_yield Token{TokenType::LBRACE, std::variant{std::monostate}};
                continue;
            }
            if (peek('}')) {
                co_yield Token{TokenType::RBRACE, std::variant{std::monostate}};
                continue;
            }
            if (peek(';')) {
                co_yield Token{TokenType::SEMICOLON, std::variant{std::monostate}};
                continue;
            }
            if (peek("int")) {
                co_yield Token{TokenType::INTEGER_TYPE, std::variant{std::monostate}};
                continue;
            }
            if (peek("return")) {
                co_yield 
                continue;
            }
            //TODO: handle int_min and int_max
            std::string value;
            if (isdigit(code_[idx])) {
                while (idx < code_.size() && is_digit(code_[idx])) {
                    value.append(code[idx]);
                    idx++;
                }
                co_yield Token{TokenType::INTEGER_VALUE, std::variant{std::stoi(value)}};
                continue;
            }
            if (peek('"')) {
                idx++;
                while(!peek('"')) {
                    values.push_back(code_[idx]);
                }
                idx++;
                co_yield Token{TokenType::TEXT, std::variant{value}};
                continue;
            }
            throw std::runtime_error("unable to lex remainder of string");
        }
    }
private:
    int idx;
    const std::string& code_;
    const std::set<char> skippable_characters_ = {'\n', ' '};
    bool peek(std::string& expected) {
        if (expected.size() + idx > code_.size()) {
            return false;
        }
        return code_.substr(idx, expected.size()) == expected;
    }
    bool peek(char c) {
        if (idx >= code_.size()) {
            return false;
        }
        return code_[idx] == c;
    }
};