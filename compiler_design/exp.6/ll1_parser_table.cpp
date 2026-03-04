#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <sstream>
#include <algorithm>

using namespace std;

// Constants for special symbols
const string EPS = "ε";

// Helper to split production strings into individual symbols
vector<string> split_prod(const string& str) {
    vector<string> tokens;
    stringstream ss(str);
    string token;
    while (ss >> token) tokens.push_back(token);
    return tokens;
}

void construct_ll1_table() {
    // 1. Define Grammar
    map<string, vector<string>> grammar;
    grammar["E"] = {"T E'"};
    grammar["E'"] = {"+ T E'", EPS};
    grammar["T"] = {"F T'"};
    grammar["T'"] = {"* F T'", EPS};
    grammar["F"] = {"( E )", "id"};

    // 2. Define FIRST sets (from Lab 5)
    map<string, set<string>> FIRST;
    FIRST["E"] = {"(", "id"};
    FIRST["E'"] = {"+", EPS};
    FIRST["T"] = {"(", "id"};
    FIRST["T'"] = {"*", EPS};
    FIRST["F"] = {"(", "id"};

    // 3. Define FOLLOW sets (from Lab 5)
    map<string, set<string>> FOLLOW;
    FOLLOW["E"] = {")", "$"};
    FOLLOW["E'"] = {")", "$"};
    FOLLOW["T"] = {"+", ")", "$"};
    FOLLOW["T'"] = {"+", ")", "$"};
    FOLLOW["F"] = {"*", "+", ")", "$"};

    // 4. Construct the Parsing Table M[Non-Terminal, Terminal]
    map<pair<string, string>, string> table;

    for (auto it = grammar.begin(); it != grammar.end(); ++it) {
        string nt = it->first;
        vector<string> productions = it->second;

        for (size_t p = 0; p < productions.size(); ++p) {
            string production = productions[p];
            vector<string> symbols = split_prod(production);
            string first_symbol = symbols[0];

            // Case 1: Production starts with terminal
            if (grammar.find(first_symbol) == grammar.end() && first_symbol != EPS) {
                table[{nt, first_symbol}] = production;
            }
            // Case 2: Production starts with non-terminal
            else if (grammar.find(first_symbol) != grammar.end()) {
                set<string> first_of_symbol = FIRST[first_symbol];
                for (auto f_it = first_of_symbol.begin(); f_it != first_of_symbol.end(); ++f_it) {
                    if (*f_it != EPS) {
                        table[{nt, *f_it}] = production;
                    }
                }
            }

            // Case 3: Epsilon production or production that can derive Epsilon
            bool can_be_epsilon = (first_symbol == EPS);
            if (grammar.find(first_symbol) != grammar.end() && FIRST[first_symbol].count(EPS)) {
                can_be_epsilon = true;
            }

            if (can_be_epsilon) {
                set<string> follow_of_nt = FOLLOW[nt];
                for (auto fo_it = follow_of_nt.begin(); fo_it != follow_of_nt.end(); ++fo_it) {
                    table[{nt, *fo_it}] = EPS;
                }
            }
        }
    }

    // 5. Print the Table
    cout << "--- LAB 6: PREDICTIVE PARSING TABLE (LL(1)) ---\n\n";
    cout << "M[Non-Terminal, Terminal] = Production\n";
    cout << "-------------------------------------------\n";
    
    for (auto const& entry : table) {
        cout << "M[" << entry.first.first << ", " << entry.first.second << "] = " 
             << entry.first.first << " -> " << entry.second << endl;
    }
}

int main() {
    construct_ll1_table();
    return 0;
}