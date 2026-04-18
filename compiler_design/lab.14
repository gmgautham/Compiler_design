#ifndef DATA_FLOW_ANALYSIS_HPP
#define DATA_FLOW_ANALYSIS_HPP

#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <algorithm>

/**
 * Basic Block: A sequence of instructions with one entry and one exit.
 */
struct BasicBlock {
    int id;
    std::vector<std::vector<std::string>> instructions;
    std::set<int> predecessors;
    std::set<int> successors;

    BasicBlock(int _id) : id(_id) {}
};

/**
 * Control Flow Graph (CFG)
 */
class ControlFlowGraph {
public:
    std::map<int, BasicBlock*> blocks;
    int block_counter = 0;

    BasicBlock* add_block() {
        BasicBlock* block = new BasicBlock(block_counter);
        blocks[block_counter++] = block;
        return block;
    }

    void add_edge(int from, int to) {
        blocks[from]->successors.insert(to);
        blocks[to]->predecessors.insert(from);
    }
};

/**
 * Reaching Definitions Analysis (Forward Analysis)
 * IN[B] = Union(OUT[P]) for all P in Predecessors(B)
 * OUT[B] = GEN[B] U (IN[B] - KILL[B])
 */
class ReachingDefinitions {
private:
    ControlFlowGraph& cfg;
    std::map<int, std::set<std::string>> gen, kill, in_set, out_set;

public:
    ReachingDefinitions(ControlFlowGraph& _cfg) : cfg(_cfg) {}

    void analyze() {
        // Initial GEN/KILL computation
        for (auto const& [id, block] : cfg.blocks) {
            std::set<std::string> block_gen;
            for (auto const& instr : block->instructions) {
                if (instr.size() >= 2) block_gen.insert(instr.back()); // Variable defined
            }
            gen[id] = block_gen;
            // Simplified KILL: In this example, assume any definition kills others of same var
            kill[id] = {}; 
        }

        bool changed = true;
        while (changed) {
            changed = false;
            for (auto const& [id, block] : cfg.blocks) {
                // IN[B] = Union of OUT of predecessors
                std::set<std::string> new_in;
                for (int pred : block->predecessors) {
                    new_in.insert(out_set[pred].begin(), out_set[pred].end());
                }
                in_set[id] = new_in;

                // OUT[B] = GEN U (IN - KILL)
                std::set<std::string> new_out = gen[id];
                for (auto const& def : in_set[id]) {
                    if (kill[id].find(def) == kill[id].end()) new_out.insert(def);
                }

                if (new_out != out_set[id]) {
                    out_set[id] = new_out;
                    changed = true;
                }
            }
        }
    }

    void print() {
        std::cout << "\nREACHING DEFINITIONS\n" << std::string(30, '=') << "\n";
        for (auto const& [id, block] : cfg.blocks) {
            std::cout << "Block " << id << " | IN: { ";
            for (auto const& s : in_set[id]) std::cout << s << " ";
            std::cout << "} | OUT: { ";
            for (auto const& s : out_set[id]) std::cout << s << " ";
            std::cout << "}\n";
        }
    }
};

/**
 * Live Variable Analysis (Backward Analysis)
 * OUT[B] = Union(IN[S]) for all S in Successors(B)
 * IN[B] = USE[B] U (OUT[B] - DEF[B])
 */
class LiveVariableAnalysis {
private:
    ControlFlowGraph& cfg;
    std::map<int, std::set<std::string>> use, def, in_set, out_set;

public:
    LiveVariableAnalysis(ControlFlowGraph& _cfg) : cfg(_cfg) {}

    void analyze() {
        // Compute USE and DEF sets
        for (auto const& [id, block] : cfg.blocks) {
            std::set<std::string> block_use, block_def;
            for (auto const& instr : block->instructions) {
                // Simplified: instr[0] and instr[1] are used, instr.back() is defined
                if (instr.size() >= 3) {
                    if (block_def.find(instr[0]) == block_def.end()) block_use.insert(instr[0]);
                    if (block_def.find(instr[1]) == block_def.end()) block_use.insert(instr[1]);
                    block_def.insert(instr.back());
                }
            }
            use[id] = block_use;
            def[id] = block_def;
        }

        bool changed = true;
        while (changed) {
            changed = false;
            // Live Variables is a BACKWARD analysis
            for (auto it = cfg.blocks.rbegin(); it != cfg.blocks.rend(); ++it) {
                int id = it->first;
                BasicBlock* block = it->second;

                // OUT[B] = Union of IN of successors
                std::set<std::string> new_out;
                for (int succ : block->successors) {
                    new_out.insert(in_set[succ].begin(), in_set[succ].end());
                }
                out_set[id] = new_out;

                // IN[B] = USE U (OUT - DEF)
                std::set<std::string> new_in = use[id];
                for (auto const& v : out_set[id]) {
                    if (def[id].find(v) == def[id].end()) new_in.insert(v);
                }

                if (new_in != in_set[id]) {
                    in_set[id] = new_in;
                    changed = true;
                }
            }
        }
    }

    void print() {
        std::cout << "\nLIVE VARIABLE ANALYSIS\n" << std::string(30, '=') << "\n";
        for (auto const& [id, block] : cfg.blocks) {
            std::cout << "Block " << id << " | IN: { ";
            for (auto const& s : in_set[id]) std::cout << s << " ";
            std::cout << "} | OUT: { ";
            for (auto const& s : out_set[id]) std::cout << s << " ";
            std::cout << "}\n";
        }
    }
};

#endif
