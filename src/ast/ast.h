#include <vector>
#include <list>
#include <string>
#include "src/ast/type.h"
#include <variant>

#pragma once

class Visitor;
struct TypeNode;
struct FunctionArgumentsNode;
struct StatementNode;
struct StatementBlockNode;
struct FunctionNode;
struct ProgramNode;

class Visitor {
public:
    virtual ~Visitor() = default;
    virtual void visit(TypeNode& node) = 0;
    virtual void visit(FunctionArgumentsNode& node) = 0;
    virtual void visit(StatementNode& node) = 0;
    virtual void visit(StatementBlockNode& node) = 0;
    virtual void visit(FunctionNode& node) = 0;
    virtual void visit(ProgramNode& node) = 0;
};

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void accept(Visitor& v) = 0;
};

typedef std::variant<int> Value;

enum StatementType {
    RETURN_STATEMENT,
};

struct ReturnParameters {
    Type return_type_;
    Value return_value_;
};

typedef std::variant<std::monostate, ReturnParameters> StatementParameters;

struct FunctionArgument {
    Type type;
    std::string name;
};


struct TypeNode: public ASTNode {
public:
    Type return_type_;
    void accept(Visitor& v) override {
        v.visit(*this);
    }
    TypeNode(Type return_type_): return_type_(return_type_) {}
};


struct FunctionArgumentsNode: public ASTNode {
public:
    std::list<FunctionArgument> arguments_;
    void accept(Visitor& v) override {
        v.visit(*this);
    }
    FunctionArgumentsNode() = default;
    FunctionArgumentsNode(std::list<FunctionArgument> &&arguments_) : arguments_(std::move(arguments_)) {}
};

struct StatementNode: public ASTNode {
public:
    StatementType type_;
    StatementParameters parameters_;
    StatementNode(StatementType type, StatementParameters parameters) : type_(type), parameters_(parameters) {}
    void accept(Visitor& v) override {
        v.visit(*this);
    }
};

struct StatementBlockNode: public ASTNode {
public:
    std::vector<StatementNode> statements_;
    StatementBlockNode() = default;
    StatementBlockNode(std::vector<StatementNode> statements) : statements_(std::move(statements)) {}
    void accept(Visitor& v) override {
        v.visit(*this);
    }
};

struct FunctionNode: public ASTNode {
public:
    std::string name_;
    TypeNode return_type_;
    FunctionArgumentsNode arguments_;
    StatementBlockNode body_;
    FunctionNode(std::string name, TypeNode return_type, FunctionArgumentsNode arguments, StatementBlockNode body)
        : name_(std::move(name)), return_type_(std::move(return_type)), arguments_(std::move(arguments)), body_(std::move(body)) {}
    void accept(Visitor& v) override {
        v.visit(*this);
    }
};

struct ProgramNode: public ASTNode {
public:
    std::vector<FunctionNode> functions_;
    ProgramNode() = default;
    ProgramNode(std::vector<FunctionNode> functions) : functions_(std::move(functions)) {}
    void accept(Visitor& v) override {
        v.visit(*this);
    }
};