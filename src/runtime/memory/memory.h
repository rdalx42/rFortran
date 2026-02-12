
// Memory management for the custom runtime environment
// all values are stored as numbers, either them beeing actual numbers or them beeing number pointers to strings or fixed arraya 

#ifndef MEMORY_H
#define MEMORY_H

#include <cstdint>
#include <climits>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include "hasher.h"

#pragma GCC optimize("Ofast","unroll-loops","fast-math")

#define MAX_MEM UINT8_MAX
#define MAX_CALL_STACK 100
#define MAX_ENUM 20
#define MAX_PARAM 40

enum class VALUE_TYPE : uint8_t{
    NUMBER,
    NONE,
    STRING,
    ARRAY,
    ENUM_OBJECT,
};

struct VALUE{

    union {
        double number_value; 
        uint16_t string_pointer_to_string_hash_array; // will pre computed string hash array later
        uint8_t array_pointer; // will pre compute later 
        struct {
            uint8_t type_id; // which enum type
            uint8_t value_id; // which value in that enum
        } enum_data;
        
    } data;

    VALUE_TYPE value_type = VALUE_TYPE::NONE;
};



struct CALL_STACK{
    public: // return value will be placed in the 0th register
    uint16_t call_stack[MAX_CALL_STACK]; // return cursor positions
    
    inline void push(uint16_t val){
        if(csp>=MAX_CALL_STACK){
            throw_error("Call stack overflow, max call stack is: " + std::to_string(MAX_CALL_STACK)+"!");
        }
        call_stack[csp++] = val;

    }
    
    inline uint16_t pop_ret(){
        if(csp<=0){
            throw_error("Call stack underflow, no return address to pop!");
        }
        return call_stack[--csp];
    }

    private:
    int csp = 0; // call stack pointer
   
};

struct PARAM_STACK {
    VALUE param_values[MAX_PARAM];
    int psp = 0;      // number of elements in the stack
    int fpsp = 0;     // index of the front element
    int maxpsp = MAX_PARAM; // max size of the stack

    inline void push(const VALUE& val) {
        if (psp >= maxpsp) {
            throw_error("Parameter stack overflow!");
        }
        int idx = (fpsp + psp) % maxpsp; 
        param_values[idx] = val;
        psp++;
    }

    inline bool empty() const {
        return psp == 0;
    }

    inline VALUE& pop_ret() {
        if (psp == 0) {
            throw_error("Parameter stack underflow, no parameter to pop!");
        }
        psp--;
        int idx = (fpsp + psp) % maxpsp;
        return param_values[idx];
    }

    inline void pop() {
        if (psp == 0) {
            throw_error("Parameter stack underflow, no parameter to pop!");
        }
        psp--;
    }

    inline VALUE& front() {
        if (psp == 0) {
            throw_error("Parameter stack is empty, no parameter at front!");
        }
        return param_values[fpsp];
    }

    inline void pop_front() {
        if (psp == 0) {
            throw_error("Parameter stack underflow, no parameter to pop at front!");
        }
        fpsp = (fpsp + 1) % maxpsp;
        psp--;
    }
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
            case VALUE_TYPE::STRING:
                std::cout<<"[Top of Stack] Type: STRING Value: "<<tval.data.string_pointer_to_string_hash_array<<"\n";
                break;
            case VALUE_TYPE::ENUM_OBJECT:
                std::cout<<"[Top of Stack] Type: ENUM_OBJECT (Type "<<(int)tval.data.enum_data.type_id<<", Value "<<(int)tval.data.enum_data.value_id<<")\n";
                break;
            case VALUE_TYPE::ARRAY:
                std::cout<<"[Top of Stack] Type: ARRAY (Addr "<<(int)tval.data.array_pointer<<")\n";
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
    VALUE array_memory[MAX_MEM][MAX_MEM]; // unchanged
    std::vector<std::vector<VALUE>> enum_memory; // dynamic enum memory

    STACK st;
    CALL_STACK call_stack;
    PARAM_STACK param_stack;
    

    STRING_HASHER* string_hasher = nullptr;
    GOTO_HASHER* goto_hasher = nullptr;

    void init(STRING_HASHER& sh, GOTO_HASHER& gh, std::unordered_map<int, std::vector<int>>& pre_init_enum_map) {
        string_hasher = &sh;
        goto_hasher = &gh;

        // Resize outer vector to number of enums
        enum_memory.resize(pre_init_enum_map.size());

        for (const auto& [type_id, values] : pre_init_enum_map) {
            // Resize inner vector to number of enum elements
            
            if(type_id+1>MAX_ENUM){
                throw_error("Invalid number of enums declared");
            }
            
            if (type_id+1 >= enum_memory.size()) {
                enum_memory.resize(type_id + 1);
            }

            if(values.size()+1>MAX_ENUM){
                throw_error("Invalid number of enum members declared");
            }

            enum_memory[type_id].resize(values.size());
           // std::cout<<type_id<<"<<\n";
            
            for (size_t val_idx = 0; val_idx <enum_memory[type_id].size(); val_idx++) {
                VALUE v;
                v.value_type = VALUE_TYPE::ENUM_OBJECT;
                v.data.enum_data.type_id = type_id;
                v.data.enum_data.value_id = val_idx;
            //    std::cout<<type_id<<"\n";
              
                enum_memory[type_id][val_idx] = v;
            }
        }

        // for(int i = 0;i<enum_memory.size();i++){
        //     std::cout<<"key : "<< i << " elements: [";
        //     for(int j = 0 ; j < enum_memory[i].size() ; j ++ ){
        //         std::cout<<(int)enum_memory[i][j].data.enum_data.value_id<<" ";
        //     }std::cout<<"]\n";
        // }
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

            case VALUE_TYPE::ENUM_OBJECT: {
                uint8_t type_id = tval.data.enum_data.type_id;
                uint8_t value_id = tval.data.enum_data.value_id;

                std::cout << "[memory at " << static_cast<int>(Pos) << "] Type: ENUM_OBJECT "
                        << "(Type " << static_cast<int>(type_id)
                        << ", Value " << static_cast<int>(value_id) << ")\n";
                break;
            }
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
