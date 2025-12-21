#include <vector>
#include "src/lexer/token.h"
#include <generator>
#include <set>

class Lexer {
public:
    virtual std::generator<Token> tokenize() = 0;
    virtual ~Lexer() = default;
};

class ManualLexer: public Lexer {
public:
    ManualLexer(const std::string code);
    auto tokenize() -> std::generator<Token> override;
    ~ManualLexer() override = default;
private:
    size_t idx_ = 0;
    const std::string code_;
    const std::set<char> skippable_characters_ = {'\n', ' '};
    bool peek(std::string expected);
    bool peek(char c);
};