#ifndef PARSER_HPP
#define PARSER_HPP

#include "ast_nodes.hpp"
#include "lexer.hpp"

class Parser {
    std::vector<Token> tokens;
    size_t pos = 0;

    Token curr() { return tokens[pos]; }
    Token eat(TokenType type) {
        if (curr().type == type) return tokens[pos++];
        throw std::runtime_error("Unexpected Token");
    }

public:
    Parser(std::vector<Token> t) : tokens(t) {}

    std::unique_ptr<ProgramNode> parse() {
        auto prog = std::make_unique<ProgramNode>();
        while (curr().type != END) {
            if (curr().value == "int") prog->statements.push_back(parseDecl());
            else if (curr().value == "print") prog->statements.push_back(parsePrint());
        }
        return prog;
    }

    std::unique_ptr<DeclarationNode> parseDecl() {
        eat(KEYWORD);
        std::string name = eat(IDENTIFIER).value;
        eat(ASSIGN);
        auto val = parseExpr();
        eat(SEMICOLON);
        return std::make_unique<DeclarationNode>(name, std::move(val));
    }

    std::unique_ptr<PrintNode> parsePrint() {
        eat(KEYWORD); eat(LPAREN);
        auto expr = parseExpr();
        eat(RPAREN); eat(SEMICOLON);
        return std::make_unique<PrintNode>(std::move(expr));
    }

    std::unique_ptr<ExprNode> parseExpr() {
        auto left = parseTerm();
        while (curr().type == PLUS || curr().type == MINUS) {
            std::string op = eat(curr().type).value;
            auto right = parseTerm();
            left = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
        }
        return left;
    }

    std::unique_ptr<ExprNode> parseTerm() {
        if (curr().type == NUMBER) return std::make_unique<NumberNode>(std::stoi(eat(NUMBER).value));
        if (curr().type == IDENTIFIER) return std::make_unique<VariableNode>(eat(IDENTIFIER).value);
        throw std::runtime_error("Invalid Term");
    }
};

#endif