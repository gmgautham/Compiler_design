#include "lexer.hpp"
#include "parser.hpp"
#include "ir_generator.hpp"

int main() {
    std::string code = "int x = 10; print(x + 5);";
    
    Lexer lexer(code);
    auto tokens = lexer.tokenize();

    Parser parser(tokens);
    auto ast = parser.parse();

    IRGenerator ir;
    ir.generate(ast.get());
    
    std::cout << "--- Intermediate Representation ---\n";
    ir.dump();

    return 0;
}