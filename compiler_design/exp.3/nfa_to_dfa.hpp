#ifndef NFA_TO_DFA_HPP
#define NFA_TO_DFA_HPP

#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <stack>
#include <string>
#include <algorithm>

// Defining Epsilon to match your Lab 2/3 requirements
const std::string EPS_SYM = "ε";

typedef int NFAState;
typedef std::set<NFAState> DFAState; // A DFA state is a set of NFA states

class SubsetConstruction {
private:
    // NFA Map: CurrentState -> (InputSymbol -> Set of NextStates)
    std::map<NFAState, std::map<std::string, std::set<NFAState>>> nfa;
    NFAState nfa_start;
    std::set<NFAState> nfa_accept;

public:
    SubsetConstruction(std::map<NFAState, std::map<std::string, std::set<NFAState>>> transitions, 
                       NFAState start, std::set<NFAState> accept) 
        : nfa(transitions), nfa_start(start), nfa_accept(accept) {}

    // Computes the epsilon closure (all states reachable via epsilon transitions)
    std::set<NFAState> compute_epsilon_closure(std::set<NFAState> states) {
        std::stack<NFAState> st;
        for (std::set<NFAState>::iterator it = states.begin(); it != states.end(); ++it) {
            st.push(*it);
        }
        
        std::set<NFAState> closure = states;
        while (!st.empty()) {
            NFAState curr = st.top();
            st.pop();

            // Access transitions for the current state
            if (nfa.count(curr) && nfa[curr].count(EPS_SYM)) {
                std::set<NFAState> next_states = nfa[curr][EPS_SYM];
                for (std::set<NFAState>::iterator it = next_states.begin(); it != next_states.end(); ++it) {
                    if (closure.find(*it) == closure.end()) {
                        closure.insert(*it);
                        st.push(*it);
                    }
                }
            }
        }
        return closure;
    }

    // Moves from a set of states on a specific input symbol
    std::set<NFAState> move(const std::set<NFAState>& states, std::string symbol) {
        std::set<NFAState> result;
        for (std::set<NFAState>::iterator it = states.begin(); it != states.end(); ++it) {
            NFAState s = *it;
            if (nfa.count(s) && nfa[s].count(symbol)) {
                std::set<NFAState> next_states = nfa[s][symbol];
                for (std::set<NFAState>::iterator n_it = next_states.begin(); n_it != next_states.end(); ++n_it) {
                    result.insert(*n_it);
                }
            }
        }
        return result;
    }

    void convert() {
        // 1. Discover the Alphabet (all symbols except epsilon)
        std::set<std::string> alphabet;
        for (auto it = nfa.begin(); it != nfa.end(); ++it) {
            std::map<std::string, std::set<NFAState>>& symbols = it->second;
            for (auto sym_it = symbols.begin(); sym_it != symbols.end(); ++sym_it) {
                if (sym_it->first != EPS_SYM) {
                    alphabet.insert(sym_it->first);
                }
            }
        }

        // 2. Initialize DFA states and transitions
        std::vector<DFAState> dfa_states;
        std::map<std::pair<DFAState, std::string>, DFAState> dfa_transitions;
        
        DFAState start_node = compute_epsilon_closure({nfa_start});
        dfa_states.push_back(start_node);

        std::stack<DFAState> unmarked;
        unmarked.push(start_node);

        std::cout << "\n--- DFA Generation Progress ---\n";
        while (!unmarked.empty()) {
            DFAState current = unmarked.top();
            unmarked.pop();

            for (std::set<std::string>::iterator sym_it = alphabet.begin(); sym_it != alphabet.end(); ++sym_it) {
                std::string sym = *sym_it;
                DFAState next_node = compute_epsilon_closure(move(current, sym));
                
                if (next_node.empty()) continue;

                dfa_transitions[std::make_pair(current, sym)] = next_node;

                // Check if this subset of states has been discovered yet
                bool is_new = true;
                for (size_t i = 0; i < dfa_states.size(); ++i) {
                    if (dfa_states[i] == next_node) {
                        is_new = false;
                        break;
                    }
                }

                if (is_new) {
                    dfa_states.push_back(next_node);
                    unmarked.push(next_node);
                }

                // Visual Output
                print_state(current);
                std::cout << " --" << sym << "--> ";
                print_state(next_node);
                std::cout << "\n";
            }
        }
    }

    void print_state(const DFAState& s) {
        std::cout << "{ ";
        for (std::set<NFAState>::iterator it = s.begin(); it != s.end(); ++it) {
            std::cout << *it << " ";
        }
        std::cout << "}";
    }
};

#endif