
#include "../lexer/lexer.h"
#include "../compiler/compiler.h"
#include "../parser/ast.h"
#include <iostream>
#include <fstream>

int main(void){
 
    LEXER lexer;
    lexer.init("runtime/main.rf");
    COMPILER compiler;
    AST ast;
    
    ast.init(lexer.tokens);

    LEXER blexer;
    blexer.init(ast.bytecode,true);
    
    compiler.memory.init(ast.string_hasher,ast.goto_hasher,ast.enum_map); 

    compiler.init(blexer.btokens); 
    
    return 0;
}
