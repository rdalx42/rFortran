
// Memory management for the custom runtime environment
// all values are stored as numbers, either them beeing actual numbers or them beeing number pointers to strings or fixed arraya 

#ifndef MEMORY_H
#define MEMORY_H

#include <cstdint>
#include <climits>
#include <iostream>

#pragma GCC optimize("Ofast","unroll-loops","fast-math")

#define MAX_MEM UINT8_MAX

struct VALUE{

    enum class VALUE_TYPE : uint8_t{
        NUMBER,
        NONE,
    };

    union {
        double number_value;
        uint16_t string_pointer_to_string_hash_array; // will pre computed string hash array later
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
            case VALUE::VALUE_TYPE::NUMBER:
                std::cout<<"[Top of Stack] Type: NUMBER Value: "<<tval.data.number_value<<"\n";
                break;
            case VALUE::VALUE_TYPE::NONE:
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
    STACK st;

    inline void store(const uint8_t& addr, const VALUE& val){
        memory[addr] = val;
    }

    inline void load(const uint8_t& addr){
        st.push(memory[addr]);
    }

    inline void list_at(uint8_t Pos){
        const VALUE& tval = memory[Pos];
        
        switch(tval.value_type){
            case VALUE::VALUE_TYPE::NUMBER:
                std::cout<<"[memory at "<<static_cast<int>(Pos)<<"] Type: NUMBER Value: "<<tval.data.number_value<<"\n";
                break;
            case VALUE::VALUE_TYPE::NONE:
                std::cout<<"[memory at "<<static_cast<int>(Pos)<<"] Type: NONE\n";
                break;
            default:
                std::cout<<"[memory at "<<static_cast<int>(Pos)<<"] Type: UNKNOWN\n";
                break;
        }
    }
};

#endif 