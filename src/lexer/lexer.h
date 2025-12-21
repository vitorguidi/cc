#include <vector>
#include "src/lexer/token.h"
#include <generator>
#include <set>
#include <unordered_map>
#include <ranges>
#include <memory>

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

class Lexer {
public:
    virtual ~Lexer() = default;
    virtual auto Lex() -> TokenStream = 0;
};

class ManualLexer: public Lexer {
public:
    ManualLexer(const std::string code);
    ~ManualLexer() override = default;
    auto Lex() -> TokenStream override;
private:
    size_t idx_ = 0;
    const std::string code_;
    const std::set<char> skippable_characters_ = {'\n', ' '};
    bool peek(std::string expected);
    bool peek(char c);
    bool skip();
    auto tokenize() -> std::generator<Token>;
};