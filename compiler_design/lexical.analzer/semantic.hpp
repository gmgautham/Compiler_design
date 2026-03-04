#ifndef SEMANTIC_ANALYZER_HPP
#define SEMANTIC_ANALYZER_HPP

#include "ast_nodes.hpp"
#include <unordered_map>
#include <string>
#include <stdexcept>
#include <vector>

class SemanticAnalyzer {
private:
    // The Symbol Table stores variable names that have been declared.
    // In a real compiler, this would also store types (int, float, etc.).
    std::unordered_map<std::string, std::string> symbolTable;

    // Helper function to check expressions recursively
    void checkExpression(ExprNode* expr) {
        if (!expr) return;

        // 1. If it's a Variable, check if it exists in the symbol table
        if (auto var = dynamic_cast<VariableNode*>(expr)) {
            if (symbolTable.find(var->name) == symbolTable.end()) {
                throw std::runtime_error("Semantic Error: Variable '" + var->name + "' used before declaration.");
            }
        }
        // 2. If it's a Binary Operation (like + or -), check both sides
        else if (auto binOp = dynamic_cast<BinaryOpNode*>(expr)) {
            checkExpression(binOp->left.get());
            checkExpression(binOp->right.get());
        }
        // 3. Numbers are always valid, so we don't need to do anything for NumberNode
    }

public:
    void analyze(ASTNode* node) {
        if (!node) return;

        // Case 1: The Root Program Node
        if (auto prog = dynamic_cast<ProgramNode*>(node)) {
            for (auto& stmt : prog->statements) {
                analyze(stmt.get());
            }
        }
        // Case 2: Declaration Node (e.g., int x = 10;)
        else if (auto decl = dynamic_cast<DeclarationNode*>(node)) {
            // Check if variable is already declared (Prevent Redefinition)
            if (symbolTable.find(decl->name) != symbolTable.end()) {
                throw std::runtime_error("Semantic Error: Variable '" + decl->name + "' is already declared.");
            }
            
            // First analyze the value being assigned
            checkExpression(decl->value.get());
            
            // Add to symbol table
            symbolTable[decl->name] = "int";
        }
        // Case 3: Print Node (e.g., print(x + 5);)
        else if (auto prnt = dynamic_cast<PrintNode*>(node)) {
            checkExpression(prnt->expression.get());
        }
    }
};

#endif