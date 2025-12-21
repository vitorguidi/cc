#include <vector>
#include <src/lexer/token.h>
#include <generator>

class Lexer {
    std::generator<Token> tokenize() = 0;
    virtual ~Lexer() = 0;
}