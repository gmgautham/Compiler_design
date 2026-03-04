#ifndef IR_GENERATOR_HPP
#define IR_GENERATOR_HPP

#include "ast_nodes.hpp"
#include <iostream>

struct IRInstr {
    std::string res, op, a1, a2;
};

class IRGenerator {
    int temp_count = 0;
    std::vector<IRInstr> instrs;

    std::string new_temp() { return "t" + std::to_string(++temp_count); }

public:
    std::string gen_expr(ExprNode* node) {
        if (auto n = dynamic_cast<NumberNode*>(node)) return std::to_string(n->value);
        if (auto v = dynamic_cast<VariableNode*>(node)) return v->name;
        if (auto b = dynamic_cast<BinaryOpNode*>(node)) {
            std::string l = gen_expr(b->left.get());
            std::string r = gen_expr(b->right.get());
            std::string t = new_temp();
            instrs.push_back({t, b->op, l, r});
            return t;
        }
        return "";
    }

    void generate(ProgramNode* prog) {
        for (auto& stmt : prog->statements) {
            if (auto d = dynamic_cast<DeclarationNode*>(stmt.get())) {
                std::string val = gen_expr(d->value.get());
                instrs.push_back({d->name, "=", val, ""});
            } else if (auto p = dynamic_cast<PrintNode*>(stmt.get())) {
                std::string val = gen_expr(p->expression.get());
                instrs.push_back({"print", "call", val, ""});
            }
        }
    }

    void dump() {
        for (auto& i : instrs) {
            if (i.op == "=") std::cout << i.res << " = " << i.a1 << "\n";
            else if (i.op == "call") std::cout << i.res << " " << i.a1 << "\n";
            else std::cout << i.res << " = " << i.a1 << " " << i.op << " " << i.a2 << "\n";
        }
    }
};

#endif