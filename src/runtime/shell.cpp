
#include "../lexer/lexer.h"
#include "../parser/parser.h"
#include "../compiler/compiler.h"
#include <iostream>
#include <fstream>

#include <chrono>

int main(void){

    LEXER lexer;
    lexer.init("runtime/main.rf");
    PARSER parser;
    
    parser.init(lexer.tokens);
    LEXER blexer;
    blexer.init(parser.bytecode,true);
    COMPILER compiler;
    compiler.memory.init(parser.string_hasher);
    compiler.init(blexer.btokens);

    return 0;
}
