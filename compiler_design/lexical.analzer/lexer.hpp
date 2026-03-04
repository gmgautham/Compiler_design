#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <vector>
#include <regex>

enum TokenType { KEYWORD, IDENTIFIER, NUMBER, ASSIGN, PLUS, MINUS, SEMICOLON, LPAREN, RPAREN, END };

struct Token {
    TokenType type;
    std::string value;
};

class Lexer {
    std::string src;
    size_t pos = 0;
public:
    Lexer(std::string s) : src(s) {}

    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        std::regex pattern(R"(int|print|[a-zA-Z_][a-zA-Z0-9_]*|\d+|\+|-|=|\(|\)|;|[ \t\n]+)");
        auto words_begin = std::sregex_iterator(src.begin(), src.end(), pattern);
        auto words_end = std::sregex_iterator();

        for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
            std::string match = i->str();
            if (isspace(match[0])) continue;

            if (match == "int" || match == "print") tokens.push_back({KEYWORD, match});
            else if (isdigit(match[0])) tokens.push_back({NUMBER, match});
            else if (isalpha(match[0])) tokens.push_back({IDENTIFIER, match});
            else if (match == "=") tokens.push_back({ASSIGN, match});
            else if (match == "+") tokens.push_back({PLUS, match});
            else if (match == "-") tokens.push_back({MINUS, match});
            else if (match == "(") tokens.push_back({LPAREN, match});
            else if (match == ")") tokens.push_back({RPAREN, match});
            else if (match == ";") tokens.push_back({SEMICOLON, match});
        }
        tokens.push_back({END, ""});
        return tokens;
    }
};

#endif