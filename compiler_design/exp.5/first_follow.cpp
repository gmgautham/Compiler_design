#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <sstream>
#include <algorithm>

using namespace std;

// Using the epsilon symbol consistent with your previous labs
const string EPS = "ε";

// Global data structures for Grammar, FIRST, and FOLLOW
map<string, vector<string>> grammar;
map<string, set<string>> FIRST;
map<string, set<string>> FOLLOW;
string start_symbol;

// Helper to split a production string (e.g., "T E'") into {"T", "E'"}
vector<string> split_production(const string& str) {
    vector<string> tokens;
    stringstream ss(str);
    string token;
    while (ss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

// Identifies if a symbol is a terminal
bool is_terminal(const string& symbol) {
    return grammar.find(symbol) == grammar.end() && symbol != EPS;
}

// Recursive function to compute FIRST sets
set<string> compute_first(string symbol) {
    // Return cached result if available
    if (FIRST.count(symbol) && !FIRST[symbol].empty()) {
        return FIRST[symbol];
    }

    set<string> resultSet;

    // Case 1: Terminal
    if (is_terminal(symbol)) {
        resultSet.insert(symbol);
        FIRST[symbol] = resultSet;
        return resultSet;
    }

    // Case 2: Non-terminal
    vector<string> productions = grammar[symbol];
    for (size_t i = 0; i < productions.size(); ++i) {
        vector<string> symbols = split_production(productions[i]);

        if (symbols[0] == EPS) {
            resultSet.insert(EPS);
            continue;
        }

        for (size_t j = 0; j < symbols.size(); ++j) {
            set<string> sym_first = compute_first(symbols[j]);
            
            // Add all non-epsilon elements from FIRST(sym) to FIRST(symbol)
            for (set<string>::iterator it = sym_first.begin(); it != sym_first.end(); ++it) {
                if (*it != EPS) resultSet.insert(*it);
            }

            // If epsilon is NOT in FIRST(sym), we stop processing this production
            if (sym_first.find(EPS) == sym_first.end()) {
                break;
            }
            
            // If we reached the last symbol and all had epsilon, add epsilon to FIRST(symbol)
            if (j == symbols.size() - 1) {
                resultSet.insert(EPS);
            }
        }
    }
    FIRST[symbol] = resultSet;
    return resultSet;
}

// Function to compute FOLLOW sets
void compute_follow() {
    // Rule 1: Place $ in FOLLOW(start_symbol)
    FOLLOW[start_symbol].insert("$");

    bool changed = true;
    while (changed) {
        changed = false;
        
        // Iterate through all non-terminals in the grammar
        for (map<string, vector<string>>::iterator it = grammar.begin(); it != grammar.end(); ++it) {
            string head = it->first;
            vector<string> productions = it->second;

            for (size_t p = 0; p < productions.size(); ++p) {
                vector<string> symbols = split_production(productions[p]);

                for (size_t i = 0; i < symbols.size(); ++i) {
                    string B = symbols[i];

                    if (!is_terminal(B) && B != EPS) {
                        size_t before_size = FOLLOW[B].size();

                        // Rule 2: If A -> αBβ, everything in FIRST(β) except ε is in FOLLOW(B)
                        if (i + 1 < symbols.size()) {
                            string next_sym = symbols[i + 1];
                            set<string> next_first = compute_first(next_sym);
                            
                            for (set<string>::iterator f_it = next_first.begin(); f_it != next_first.end(); ++f_it) {
                                if (*f_it != EPS) FOLLOW[B].insert(*f_it);
                            }

                            // Rule 3: If A -> αBβ and ε is in FIRST(β), then FOLLOW(A) is in FOLLOW(B)
                            if (next_first.count(EPS)) {
                                set<string> head_follow = FOLLOW[head];
                                FOLLOW[B].insert(head_follow.begin(), head_follow.end());
                            }
                        } 
                        // Rule 4: If A -> αB, then FOLLOW(A) is in FOLLOW(B)
                        else {
                            set<string> head_follow = FOLLOW[head];
                            FOLLOW[B].insert(head_follow.begin(), head_follow.end());
                        }

                        if (FOLLOW[B].size() > before_size) changed = true;
                    }
                }
            }
        }
    }
}

int main() {
    // Define Grammar: E -> T E', E' -> + T E' | ε, T -> F T', T' -> * F T' | ε, F -> ( E ) | id
    grammar["E"] = {"T E'"};
    grammar["E'"] = {"+ T E'", EPS};
    grammar["T"] = {"F T'"};
    grammar["T'"] = {"* F T'", EPS};
    grammar["F"] = {"( E )", "id"};

    start_symbol = "E";

    cout << "--- LAB 5: FIRST AND FOLLOW SETS ---\n\n";

    // 1. Compute and Print FIRST sets
    cout << "FIRST Sets:\n";
    for (map<string, vector<string>>::iterator it = grammar.begin(); it != grammar.end(); ++it) {
        string nt = it->first;
        set<string> f_set = compute_first(nt);
        cout << "FIRST(" << nt << ") = { ";
        for (set<string>::iterator s_it = f_set.begin(); s_it != f_set.end(); ++s_it)
            cout << *s_it << " ";
        cout << "}\n";
    }

    // 2. Compute and Print FOLLOW sets
    compute_follow();
    cout << "\nFOLLOW Sets:\n";
    for (map<string, vector<string>>::iterator it = grammar.begin(); it != grammar.end(); ++it) {
        string nt = it->first;
        set<string> fo_set = FOLLOW[nt];
        cout << "FOLLOW(" << nt << ") = { ";
        for (set<string>::iterator s_it = fo_set.begin(); s_it != fo_set.end(); ++s_it)
            cout << *s_it << " ";
        cout << "}\n";
    }

    return 0;
}