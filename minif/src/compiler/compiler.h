
#ifndef COMPILER_H
#define COMPILER_H

#include "../lexer/lexer.h"
#include "../runtime/memory/memory.h"

#pragma GCC optimize("Ofast","unroll-loops","fast-math")

struct REGISTERS{
    VALUE registers[16]; 
};

struct COMPILER{
    REGISTERS registers;
    MEMORY memory;
    std::vector<BTOKEN>bytecode;
    uint16_t ip=0;

    public:
        void init(const std::vector<BTOKEN>& ibytecode){
            this->bytecode=ibytecode;
            this->run();
        }
    private:
        void run();

};

#endif 