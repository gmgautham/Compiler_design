#ifndef THOMPSON_NFA_HPP
#define THOMPSON_NFA_HPP

#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include <algorithm>

const std::string EPSILON = "ε";

struct Transition {
    int from;
    std::string symbol;
    int to;
};

struct NFA {
    int start;
    int accept;
    std::vector<Transition> transitions;
};

class ThompsonConverter {
    int state_id = 0;

    int new_state() {
        return state_id++;
    }

    // Adds explicit concatenation dots (e.g., "ab" -> "a.b")
    std::string add_concat(std::string regex) {
        std::string result = "";
        for (size_t i = 0; i < regex.length(); i++) {
            result += regex[i];
            if (i + 1 < regex.length()) {
                char curr = regex[i];
                char next = regex[i+1];
                if ((isalnum(curr) || curr == ')' || curr == '*') && 
                    (isalnum(next) || next == '(')) {
                    result += '.';
                }
            }
        }
        return result;
    }

    int precedence(char op) {
        if (op == '*') return 3;
        if (op == '.') return 2;
        if (op == '|') return 1;
        return 0;
    }

    std::string to_postfix(std::string regex) {
        std::string output = "";
        std::stack<char> s;
        for (char ch : regex) {
            if (isalnum(ch)) {
                output += ch;
            } else if (ch == '(') {
                s.push(ch);
            } else if (ch == ')') {
                while (!s.empty() && s.top() != '(') {
                    output += s.top();
                    s.pop();
                }
                s.pop();
            } else {
                while (!s.empty() && precedence(s.top()) >= precedence(ch)) {
                    output += s.top();
                    s.pop();
                }
                s.push(ch);
            }
        }
        while (!s.empty()) {
            output += s.top();
            s.pop();
        }
        return output;
    }

public:
    NFA build_nfa(std::string raw_regex) {
        std::string regex = add_concat(raw_regex);
        std::string postfix = to_postfix(regex);
        std::stack<NFA> nfa_stack;

        for (char ch : postfix) {
            if (isalnum(ch)) {
                int s = new_state();
                int e = new_state();
                nfa_stack.push({s, e, {{s, std::string(1, ch), e}}});
            } else if (ch == '.') {
                NFA n2 = nfa_stack.top(); nfa_stack.pop();
                NFA n1 = nfa_stack.top(); nfa_stack.pop();
                n1.transitions.insert(n1.transitions.end(), n2.transitions.begin(), n2.transitions.end());
                n1.transitions.push_back({n1.accept, EPSILON, n2.start});
                nfa_stack.push({n1.start, n2.accept, n1.transitions});
            } else if (ch == '|') {
                NFA n2 = nfa_stack.top(); nfa_stack.pop();
                NFA n1 = nfa_stack.top(); nfa_stack.pop();
                int s = new_state();
                int e = new_state();
                std::vector<Transition> trans = n1.transitions;
                trans.insert(trans.end(), n2.transitions.begin(), n2.transitions.end());
                trans.push_back({s, EPSILON, n1.start});
                trans.push_back({s, EPSILON, n2.start});
                trans.push_back({n1.accept, EPSILON, e});
                trans.push_back({n2.accept, EPSILON, e});
                nfa_stack.push({s, e, trans});
            } else if (ch == '*') {
                NFA n = nfa_stack.top(); nfa_stack.pop();
                int s = new_state();
                int e = new_state();
                std::vector<Transition> trans = n.transitions;
                trans.push_back({s, EPSILON, n.start});
                trans.push_back({s, EPSILON, e});
                trans.push_back({n.accept, EPSILON, n.start});
                trans.push_back({n.accept, EPSILON, e});
                nfa_stack.push({s, e, trans});
            }
        }
        return nfa_stack.top();
    }
};

#endif