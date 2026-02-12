
#ifndef COMPILER_H
#define COMPILER_H

#include "../lexer/lexer.h"
#include "../runtime/stl/stl.h"
#include "../runtime/memory/memory.h"
#include <chrono>

#define MAX_REG 16

#pragma GCC optimize("Ofast","unroll-loops","fast-math")

struct REGISTERS{
    VALUE registers[MAX_REG]; 
};

struct COMPILER{
    REGISTERS registers;
    MEMORY memory;
    std::vector<BTOKEN>bytecode;
    uint16_t ip=0;
    STL standard;

    public:
        void init(const std::vector<BTOKEN>& ibytecode){
            this->bytecode=ibytecode;
            this->init_content();
            this->standard.init(this);
            this->run();
        }
    private:
        void init_content();
        void run();
        

};

#endif 
