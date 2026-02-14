#ifndef STL_H
#define STL_H

#include <string>
#include <vector>
#include <functional>
#include <array>
#include <chrono>
#include <random>
#include <limits>
#include <climits>
#include "../../runtime/memory/memory.h"

struct COMPILER;

#define NR_BUILTINS 5
#define FUNCTION_REGISTERS 4

struct STL {
public:
    COMPILER* comp; 
    const std::array<std::string, NR_BUILTINS> valid_function_names = { "List", "Time", "RandRange", "GetChar", "GetType" };
    std::function<void(bool)> builtins[NR_BUILTINS];
    
    double number_registers[FUNCTION_REGISTERS] = {0};
    char char_registers[FUNCTION_REGISTERS] ;

    void init(COMPILER* compiler);

private:
    void List(bool standalone);
    void Time(bool standalone);
    void RandRange(bool standalone);
    void GetChar(bool standalone);
    void GetType(bool standalone);
};

#endif
