#include "compiler.h"
#include "../lexer/lexer.h"
#include "../error/error.h"

void COMPILER::init_content() {
    // Iterate all bytecode tokens
    for (size_t i = 0; i < bytecode.size(); i++) {
        BTOKEN& token = bytecode[i];
        
        switch (token.token_type) {
            case BTOKEN_TYPE::LABEL: {
                uint16_t label_id = token.data.number_value;
                memory.goto_hasher->set_label_address(label_id, i);
                break;
            }

            // -----------------------------
            // Pre-validate binary operators
            // -----------------------------
            case BTOKEN_TYPE::OP: {
                if (i < 2) {
                    throw_error("Not enough operands for operator at init_content");
                }

                break;
            }

            // -----------------------------
            // AND / OR
            // -----------------------------
            case BTOKEN_TYPE::AND:
            case BTOKEN_TYPE::OR: {
                if (i < 2) {
                    throw_error("Not enough operands for AND/OR at init_content");
                }
              
                break;
            }

            default:
                break;
        }
    }

    // Finish goto mapping
    memory.goto_hasher->fill_hashed_goto_positions();
    memory.goto_hasher->list();
}

void COMPILER::run() {

    // for(int i = 0 ; i < bytecode.size();i++){

    //     if(bytecode[i].data.number_value){
    //         std::cout<<i<<". "<<bytecode_token_type_to_string(bytecode[i].token_type)<<" "<<static_cast<double>(bytecode[i].data.number_value)<<" ";
    //     }else{
    //         std::cout<<i<<". "<<bytecode_token_type_to_string(bytecode[i].token_type)<<" "<<static_cast<unsigned char>(bytecode[i].data.char_value)<<" ";
    //     }

    //     std::cout<<"\n";
    // }
    
    ip=0;
    auto start = std::chrono::high_resolution_clock::now();

    while (ip < bytecode.size()) {
        const BTOKEN& token = bytecode[ip];
       // std::cout<<ip<<"\n";
       // std::cout<<bytecode_token_type_to_string(token.token_type)<<" - "<<ip<<"\n";

        switch (token.token_type) {

            // ----------------------------------
            // PUSH literal number onto stack
            // ----------------------------------
            case BTOKEN_TYPE::PUSH: {
                registers.registers[0].value_type = VALUE_TYPE::NUMBER;
                registers.registers[0].data.number_value = token.data.number_value;
              //  std::cout<<registers.registers[0].data.number_value<<"p\n";
                memory.st.push(registers.registers[0]);
                
                ip++;
                break;
            }


            // ----------------------------------
            // LIST TOP from memory stack 
            // ----------------------------------

            case BTOKEN_TYPE::LIST:{
                this->memory.list_at(token.data.number_value);
                ip++;
                break;
            }

            // ----------------------------------
            // LOAD from memory into stack
            // ----------------------------------
            case BTOKEN_TYPE::LOAD: {
                uint16_t addr = token.data.number_value;
                registers.registers[0] = memory.memory[addr]; // load into register
                memory.st.push(registers.registers[0]);
                ip++;
                break;
            }

            // ----------------------------------
            // STORE from stack into memory
            // ----------------------------------
            case BTOKEN_TYPE::STORE: {
                uint16_t addr = token.data.number_value;
                registers.registers[0] = memory.st.pop_ret();
                memory.memory[addr] = registers.registers[0];
                ip++;
                break;
            }

            // ----------------------------------
            // ARRAY METHODS
            // ----------------------------------

            case BTOKEN_TYPE::LOAD_ARRAY:{

                // when you add functions make functions either be defined as void or no void and make it so that you cant store novoid function calls as  objects randomly placed 

                uint8_t addr = token.data.number_value;
                for(int i = memory.st.sp-1;i>=0;i--){
                    memory.array_memory[addr][i]=memory.st.pop_ret();
                }

                registers.registers[0].value_type=VALUE_TYPE::ARRAY;
                registers.registers[0].data.array_pointer = addr;

                memory.st.push(registers.registers[0]); // push the value               

                ip++;
                break;
            }

            case BTOKEN_TYPE::SET_ARRAY_AT:{
                
                registers.registers[1]=memory.st.pop_ret(); // index 
                registers.registers[0] = memory.st.pop_ret(); // value;

                if(registers.registers[1].value_type!=VALUE_TYPE::NUMBER||registers.registers[1].data.number_value>UINT8_MAX||registers.registers[1].data.number_value<0){
                    throw_error("Array index is invalid!");
                }

                memory.array_memory[(uint8_t)token.data.number_value][(uint8_t)registers.registers[1].data.number_value]=registers.registers[0];

                ip++;
                break;
            }

            case BTOKEN_TYPE::LOAD_ARRAY_AT:{

                registers.registers[0]=memory.st.pop_ret(); // index

                if(registers.registers[0].value_type!=VALUE_TYPE::NUMBER||registers.registers[0].data.number_value<0||registers.registers[0].data.number_value>UINT8_MAX){
                    throw_error("Array index is invalid!");
                }

                registers.registers[0]=memory.array_memory[(uint8_t)token.data.number_value][(uint8_t)registers.registers[0].data.number_value];
                memory.st.push(registers.registers[0]);

                ip++;
                break;
            }

            // ----------------------------------
            // Enum operations
            // ----------------------------------

            case BTOKEN_TYPE::STORE_ENUM_VALUE:{
                
                registers.registers[0]=memory.st.pop_ret(); // we know by default the type of it
                //std::cout<<(int)registers.registers[0].data.number_value<<"\n";
                //std::cout<<"ip "<<ip<<"\n";
                registers.registers[1].value_type=VALUE_TYPE::ENUM_OBJECT;
                registers.registers[1].data.enum_data.value_id=(int)token.data.number_value;
                registers.registers[1].data.enum_data.type_id=(int)registers.registers[0].data.number_value;
                //std::cout<<"pos->" <<(int)registers.registers[0].data.number_value<<" val-> "<<(int)token.data.number_value<<"\n";
                memory.enum_memory[(int)registers.registers[0].data.number_value][(int)token.data.number_value] = registers.registers[1];
                ip++;
                break;
            }

            case BTOKEN_TYPE::PUSH_ENUM_VALUE:{
                registers.registers[0]=memory.st.pop_ret(); // get enum id 
                registers.registers[1].value_type=VALUE_TYPE::ENUM_OBJECT;
                registers.registers[1].data.enum_data.value_id=(int)token.data.number_value;
                registers.registers[1].data.enum_data.type_id=(int)registers.registers[0].data.number_value;
                memory.st.push(registers.registers[1]);
                ip++;
                break;
            }
        
            // ----------------------------------
            // Binary operator: pop 2, compute, push
            // ----------------------------------
            case BTOKEN_TYPE::OP: {
                registers.registers[1] = memory.st.pop_ret(); // RHS
                registers.registers[0] = memory.st.pop_ret(); // LHS

                auto &lhs = registers.registers[0];
                auto &rhs = registers.registers[1];

                if (lhs.value_type == VALUE_TYPE::ARRAY || rhs.value_type == VALUE_TYPE::ARRAY) {
                    throw_error("Operations cannot be used on arrays");
                }

                switch(token.data.char_value) {
                    // -----------------------
                    // Arithmetic (numbers only)
                    // -----------------------
                    case '+':
                    case '-':
                    case '*':
                    case '/':
                    {
                        
                        switch(token.data.char_value) {
                            case '+': lhs.data.number_value += rhs.data.number_value; break;
                            case '-': lhs.data.number_value -= rhs.data.number_value; break;
                            case '*': lhs.data.number_value *= rhs.data.number_value; break;
                            case '/': lhs.data.number_value /= rhs.data.number_value; break;
                        }
                        break;
                    }

                    // -----------------------
                    // Comparison
                    // -----------------------
                    case '=': // ==
                    case '~': // !=
                    case '<':
                    case '>':
                    case '[': // <=
                    case ']': // >=
                    {
                        // Numbers
                        if(lhs.value_type == VALUE_TYPE::NUMBER && rhs.value_type == VALUE_TYPE::NUMBER) {
                            switch(token.data.char_value) {
                                case '=': lhs.data.number_value = lhs.data.number_value == rhs.data.number_value; break;
                                case '~': lhs.data.number_value = lhs.data.number_value != rhs.data.number_value; break;
                                case '<': lhs.data.number_value = lhs.data.number_value < rhs.data.number_value; break;
                                case '>': lhs.data.number_value = lhs.data.number_value > rhs.data.number_value; break;
                                case '[': lhs.data.number_value = lhs.data.number_value <= rhs.data.number_value; break;
                                case ']': lhs.data.number_value = lhs.data.number_value >= rhs.data.number_value; break;
                            }
                        }
                        // Strings (only == and != make sense)
                        else if(lhs.value_type == VALUE_TYPE::STRING && rhs.value_type == VALUE_TYPE::STRING) {
                            const auto &lhs_str = memory.string_hasher->hashed_strings[lhs.data.string_pointer_to_string_hash_array];
                            const auto &rhs_str = memory.string_hasher->hashed_strings[rhs.data.string_pointer_to_string_hash_array];

                            switch(token.data.char_value) {
                                case '=': lhs.data.number_value = lhs_str == rhs_str; break;
                                case '~': lhs.data.number_value = lhs_str != rhs_str; break;
                                default:
                                    throw_error("Invalid string comparison, only '!=' and '==' allowed!");
                                    break;
                            }
                        }

                        // enums : only (!= and ==)
                        else if(lhs.value_type == VALUE_TYPE::ENUM_OBJECT && rhs.value_type == VALUE_TYPE::ENUM_OBJECT) {
                            // Both must be same enum type
                            if(lhs.data.enum_data.type_id != rhs.data.enum_data.type_id) {
                                throw_error("Cannot compare enums of different types");
                            }

                            switch(token.data.char_value) {
                                case '=': lhs.data.number_value = (lhs.data.enum_data.value_id == rhs.data.enum_data.value_id); break;
                                case '~': lhs.data.number_value = (lhs.data.enum_data.value_id != rhs.data.enum_data.value_id); break;
                                default:
                                    throw_error("Invalid enum comparison, only '==' and '!=' allowed!");
                            }
                            
                            lhs.value_type = VALUE_TYPE::NUMBER; // result is always number
                        }

                      

                        lhs.value_type = VALUE_TYPE::NUMBER; // result is always number
                        break;
                    }
                }

                memory.st.push(lhs); // push result
                ip++;
                break;
            }

            // ----------------------------------
            // OR / AND
            // ----------------------------------

            case BTOKEN_TYPE::OR: {
                registers.registers[1] = memory.st.pop_ret(); // RHS
                registers.registers[0] = memory.st.pop_ret(); // LHS

                switch (registers.registers[0].value_type){
                    case VALUE_TYPE::STRING:
                        throw_error("'and' operation can only be used on numbers!");
                        break;
                    default:
                        break;
                }

                switch (registers.registers[1].value_type){
                    case VALUE_TYPE::STRING:
                        throw_error("'and' operation can only be used on numbers!");
                        break;
                    default:
                        break;
                }

                registers.registers[0].data.number_value =
                    (registers.registers[0].data.number_value != 0) ||
                    (registers.registers[1].data.number_value != 0);

                registers.registers[0].value_type = VALUE_TYPE::NUMBER;
                memory.st.push(registers.registers[0]);
                ip++;
                break;
            }

            case BTOKEN_TYPE::AND: {
                registers.registers[1] = memory.st.pop_ret();
                registers.registers[0] = memory.st.pop_ret();
                
                switch (registers.registers[0].value_type){
                    case VALUE_TYPE::STRING:
                        throw_error("'and' operation can only be used on numbers!");
                        break;
                    default:
                        break;
                }

                switch (registers.registers[1].value_type){
                    case VALUE_TYPE::STRING:
                        throw_error("'and' operation can only be used on numbers!");
                        break;
                    default:
                        break;
                }

                registers.registers[0].data.number_value =
                    (registers.registers[0].data.number_value != 0) &&
                    (registers.registers[1].data.number_value != 0);

                registers.registers[0].value_type = VALUE_TYPE::NUMBER;
                memory.st.push(registers.registers[0]);
                ip++;
                break;
            }

            // ----------------------------------
            // Load STRING
            // ----------------------------------

            case BTOKEN_TYPE::LOADSTRING:{
                uint16_t str_id = token.data.number_value;

                registers.registers[0].value_type = VALUE_TYPE::STRING;
                registers.registers[0].data.string_pointer_to_string_hash_array=str_id;
                
                memory.st.push(registers.registers[0]);
                ip++;
                break;
            }

            // ----------------------------------
            // Unary NEG
            // ----------------------------------
            case BTOKEN_TYPE::NEG: {
                registers.registers[0] = memory.st.pop_ret();
                registers.registers[0].data.number_value = -registers.registers[0].data.number_value;
                memory.st.push(registers.registers[0]);
                ip++;
                break;
            }

            // ----------------------------------
            // Unary NOT
            // ----------------------------------
            case BTOKEN_TYPE::NOT: {
                registers.registers[0] = memory.st.pop_ret();
                registers.registers[0].data.number_value = !registers.registers[0].data.number_value;
                memory.st.push(registers.registers[0]);
                ip++;
                break;
            }

            // ----------------------------------
            // GOTO IF FALSE
            // ----------------------------------

            case BTOKEN_TYPE::GOTO_IF_FALSE:{
                registers.registers[0]=memory.st.pop_ret();
                
                bool is_false = false;
                
                if (registers.registers[0].value_type == VALUE_TYPE::NUMBER) {
                    is_false = (registers.registers[0].data.number_value == 0);
                } else if (registers.registers[0].value_type == VALUE_TYPE::STRING) {
                    is_false = (memory.string_hasher->hashed_strings[registers.registers[0].data.string_pointer_to_string_hash_array].empty());
                } else {
                    throw_error("Unsupported value type in GOTO_IF_FALSE");
                }
                
                if(is_false == true){
                    uint16_t label_id = token.data.number_value;
                   // std::cout<<"going to: "<<memory.goto_hasher->hashed_goto_positions[label_id] << " from "<<ip<<'\n';
                    ip = memory.goto_hasher->hashed_goto_positions[label_id]; // jump to label position
                    
                }else{
                    ip++;
                }
                break;
            }

            case BTOKEN_TYPE::LABEL:{
                ip++;
                break;
            }

            case BTOKEN_TYPE::GOTO:{
                uint16_t label_id = token.data.number_value;
                ip = memory.goto_hasher->hashed_goto_positions[label_id]; // jump to label position
                break;
            }

            default:
                throw_error("Unknown bytecode instruction");
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    
    std::chrono::duration<double, std::milli> duration_ms = end - start;
    std::cout << "Execution time: " << duration_ms.count() << " ms\n";
    
}
