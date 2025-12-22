#include <vector>
#include <list>
#include <string>
#include "src/ast/type.h"
#include <variant>

#pragma once

class ASTNode {};

typedef std::variant<int> Value;

struct FunctionArgument {
    Type type;
    std::string name;
};


struct TypeNode: public ASTNode {
    Type return_type_;
};


struct FunctionArgumentsNode: public ASTNode {
    std::list<FunctionArgument> arguments_;
};


enum StatementType {
    RETURN_STATEMENT,
};

struct ReturnParameters {
    Type return_type_;
    Value return_value_;
};


struct StatementNode: public ASTNode {
    StatementType type_;
    std::variant<std::monostate, ReturnParameters> parameters_;
};

struct StatementBlockNode: public ASTNode {
    std::vector<StatementNode> statements_;
};

struct FunctionNode: public ASTNode {
    std::string name_;
    TypeNode return_type_;
    FunctionArgumentsNode arguments_;
    StatementBlockNode body_;
};

struct ProgramNode: public ASTNode {
    std::vector<FunctionNode> functions_;
};