#ifndef AST_NODES_HPP
#define AST_NODES_HPP

#include <string>
#include <vector>
#include <memory>

class ASTNode {
public:
    virtual ~ASTNode() = default;
};

class ExprNode : public ASTNode {};

class NumberNode : public ExprNode {
public:
    int value;
    NumberNode(int val) : value(val) {}
};

class VariableNode : public ExprNode {
public:
    std::string name;
    VariableNode(std::string n) : name(n) {}
};

class BinaryOpNode : public ExprNode {
public:
    std::string op;
    std::unique_ptr<ExprNode> left, right;
    BinaryOpNode(std::string o, std::unique_ptr<ExprNode> l, std::unique_ptr<ExprNode> r)
        : op(o), left(std::move(l)), right(std::move(r)) {}
};

class StatementNode : public ASTNode {};

class DeclarationNode : public StatementNode {
public:
    std::string name;
    std::unique_ptr<ExprNode> value;
    DeclarationNode(std::string n, std::unique_ptr<ExprNode> v)
        : name(n), value(std::move(v)) {}
};

class PrintNode : public StatementNode {
public:
    std::unique_ptr<ExprNode> expression;
    PrintNode(std::unique_ptr<ExprNode> e) : expression(std::move(e)) {}
};

class ProgramNode : public ASTNode {
public:
    std::vector<std::unique_ptr<StatementNode>> statements;
};

#endif