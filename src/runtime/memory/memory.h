
// Memory management for the custom runtime environment
// all values are stored as numbers, either them beeing actual numbers or them beeing number pointers to strings or fixed arraya 

#ifndef MEMORY_H
#define MEMORY_H

#include <cstdint>
#include <climits>
#include <iostream>
#include "hasher.h"

#pragma GCC optimize("Ofast","unroll-loops","fast-math")

#define MAX_MEM UINT8_MAX

enum class VALUE_TYPE : uint8_t{
    NUMBER,
    NONE,
    STRING,
    ARRAY,
};

struct VALUE{

    union {
        double number_value;
        uint16_t string_pointer_to_string_hash_array; // will pre computed string hash array later
        uint8_t array_pointer; // will pre compute later 
    } data;

    VALUE_TYPE value_type = VALUE_TYPE::NONE;
};

struct STACK{

    public:
        VALUE stack[MAX_MEM];
      
    int sp = 0; // stack pointer
    inline void push(const VALUE& val){
            
        stack[sp++] = val;
    }
        
    inline VALUE& pop_ret(){
        return stack[--sp];
    }

    inline void pop(){
        sp--;
    }

    inline void list_top(){
        const VALUE& tval = stack[sp-1];
        
        switch(tval.value_type){
            case VALUE_TYPE::NUMBER:
                std::cout<<"[Top of Stack] Type: NUMBER Value: "<<tval.data.number_value<<"\n";
                break;
           
            case VALUE_TYPE::NONE:
                std::cout<<"[Top of Stack] Type: NONE\n";
                break;
            default:
                std::cout<<"[Top of Stack] Type: UNKNOWN\n";
                break;
        }
    }

};

struct MEMORY{

    VALUE memory[MAX_MEM];
    VALUE array_memory[MAX_MEM][MAX_MEM]; // key -> array, only pointer value will sit in the actual memory

    STACK st;
    STRING_HASHER* string_hasher = nullptr;
    GOTO_HASHER* goto_hasher = nullptr;

    void init(STRING_HASHER& sh,GOTO_HASHER& gh){
        string_hasher = &sh;
        goto_hasher = &gh;
    }

    inline void store(const uint8_t& addr, const VALUE& val){
        memory[addr] = val;
    }

    inline void load(const uint8_t& addr){
        st.push(memory[addr]);
    }

    inline void list_at(uint8_t Pos) {
        const VALUE& tval = memory[Pos];

        switch (tval.value_type) {
            case VALUE_TYPE::NUMBER:
                std::cout << "[memory at " << static_cast<int>(Pos) << "] Type: NUMBER Value: " 
                        << tval.data.number_value << "\n";
                break;

            case VALUE_TYPE::NONE:
                std::cout << "[memory at " << static_cast<int>(Pos) << "] Type: NONE\n";
                break;

            case VALUE_TYPE::STRING:
                std::cout << "[memory at " << static_cast<int>(Pos) << "] Type: STRING Value: " 
                        << string_hasher->hashed_strings[tval.data.string_pointer_to_string_hash_array]
                        << " POINTER: " << tval.data.string_pointer_to_string_hash_array << "\n";
                break;

            case VALUE_TYPE::ARRAY: {
                uint8_t array_idx = tval.data.array_pointer;
                std::cout << "[memory at " << static_cast<int>(Pos) << "] Type: ARRAY (Addr " 
                        << static_cast<int>(array_idx) << ") Elements:\n";

                // Loop through all elements in the array
                for (int i = 0; i < UINT8_MAX; ++i) {  // You might want actual array size tracking instead of st.sp
                    const VALUE& elem = array_memory[array_idx][i];
                    if(elem.value_type==VALUE_TYPE::NONE){
                        std::cout<<"\n"; // array listing stops at first null elements
                        return;
                    }
                    std::cout << "  [" << i << "]: ";

                    // Recursive listing for nested values
                    switch (elem.value_type) {
                        case VALUE_TYPE::NUMBER:
                            std::cout << "NUMBER: " << elem.data.number_value << "\n";
                            break;
                        case VALUE_TYPE::STRING:
                            std::cout << "STRING: " 
                                    << string_hasher->hashed_strings[elem.data.string_pointer_to_string_hash_array]
                                    << "\n";
                            break;
                        case VALUE_TYPE::NONE:
                            std::cout << "NONE\n";
                            break;
                        case VALUE_TYPE::ARRAY:
                            std::cout << "ARRAY (Addr " << static_cast<int>(elem.data.array_pointer) << ")\n";
                            break;
                        default:
                            std::cout << "UNKNOWN\n";
                            break;
                    }
                }
                break;
            }

            default:
                std::cout << "[memory at " << static_cast<int>(Pos) << "] Type: UNKNOWN\n";
                break;
        }
    }

};

#endif 
