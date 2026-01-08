
#include "../lexer/lexer.h"
#include "../parser/parser.h"
#include "../compiler/compiler.h"
#include "../parser/ast.h"
#include <iostream>
#include <fstream>

int main(void){

    LEXER lexer;
    lexer.init("runtime/main.rf");
    AST ast;
    
    ast.init(lexer.tokens);

    LEXER blexer;
    blexer.init(ast.bytecode,true);
    COMPILER compiler;
    compiler.memory.init(ast.string_hasher,ast.goto_hasher);
    compiler.init(blexer.btokens);

    return 0;
}
