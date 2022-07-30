#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "symtable.hpp"
#include "ast.hpp"

extern FILE *yyin;
extern int yyparse();
extern ASTNode *ast;
extern SymTable *env;
extern std::vector<std::string> errors;

int main(int argc, char const *argv[]) {
    if (argc < 2) {
        std::cerr << "i/o unbalance: no input file" << std::endl;
        return 1;
    }
    
    yyin = NULL;
    yyin = fopen(argv[1], "r");
    if (!yyin) {
        std::cerr << "i/o unbalance: input file `" << argv[1] << "` not found" << std::endl;
        return 1;
    }
    env = new SymTable();
    yyparse();
    fclose(yyin);
    int e = errors.size();
    if (e > 0) {
        std::cerr << std::endl;
        for (auto it = errors.begin(); it != errors.end(); ++it)
            std::cerr << *it << std::endl;
        return 1;
    }
    if (ast)
        std::cout << *ast << std::endl;
    
    return 0;
}
