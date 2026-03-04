#ifndef GRAMMAR_TOOLS_HPP
#define GRAMMAR_TOOLS_HPP

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

class GrammarOptimizer {
public:
    // 1. Elimination of Immediate Left Recursion
    // Form: A -> Aa | b  =>  A -> bA', A' -> aA' | ε
    void eliminateLeftRecursion(std::string nt, std::vector<std::string> productions) {
        std::vector<std::string> alpha, beta;

        for (const std::string& prod : productions) {
            // Check if production starts with the non-terminal (Left Recursion)
            if (prod.substr(0, nt.length()) == nt) {
                alpha.push_back(prod.substr(nt.length()));
            } else {
                beta.push_back(prod);
            }
        }

        if (alpha.empty()) {
            std::cout << "No left recursion found for " << nt << ".\n";
            return;
        }

        std::string new_nt = nt + "'";

        std::cout << "\nAfter eliminating left recursion for " << nt << ":\n";
        
        // New production: A -> β1A' | β2A'
        std::cout << nt << " -> ";
        for (size_t i = 0; i < beta.size(); ++i) {
            std::cout << beta[i] << new_nt << (i == beta.size() - 1 ? "" : " | ");
        }
        std::cout << "\n";

        // New production: A' -> α1A' | α2A' | ε
        std::cout << new_nt << " -> ";
        for (const std::string& a : alpha) {
            std::cout << a << new_nt << " | ";
        }
        std::cout << "ε\n";
    }

    // 2. Left Factoring
    // Form: A -> ab1 | ab2  =>  A -> aA', A' -> b1 | b2
    void leftFactor(std::string nt, std::vector<std::string> productions) {
        if (productions.empty()) return;

        // Find common prefix
        std::string prefix = productions[0];
        for (size_t i = 1; i < productions.size(); ++i) {
            std::string current = productions[i];
            size_t j = 0;
            while (j < prefix.length() && j < current.length() && prefix[j] == current[j]) {
                j++;
            }
            prefix = prefix.substr(0, j);
        }

        if (prefix.empty()) {
            std::cout << "\nNo left factoring needed for " << nt << ".\n";
            return;
        }

        std::string new_nt = nt + "'";

        std::cout << "\nAfter left factoring for " << nt << ":\n";
        std::cout << nt << " -> " << prefix << new_nt << "\n";
        
        std::cout << new_nt << " -> ";
        for (size_t i = 0; i < productions.size(); ++i) {
            std::string suffix = productions[i].substr(prefix.length());
            std::cout << (suffix.empty() ? "ε" : suffix) << (i == productions.size() - 1 ? "" : " | ");
        }
        std::cout << "\n";
    }
};

#endif