#include "compiler.h"
#include "../error/error.h"

void COMPILER::init_goto_addrs(){
    for(size_t i=0;i<bytecode.size();i++){
        const BTOKEN& token = bytecode[i];
        if(token.token_type == BTOKEN_TYPE::LABEL){
            uint16_t label_id = token.data.number_value;
            this->memory.goto_hasher->set_label_address(label_id, i);
        }
    }
    this->memory.goto_hasher->fill_hashed_goto_positions();
    this->memory.goto_hasher->list();
}

void COMPILER::run() {

    ip=0;

    while (ip < bytecode.size()) {
        const BTOKEN& token = bytecode[ip];

        switch (token.token_type) {

            // ----------------------------------
            // PUSH literal number onto stack
            // ----------------------------------
            case BTOKEN_TYPE::PUSH: {
                registers.registers[0].value_type = VALUE_TYPE::NUMBER;
                registers.registers[0].data.number_value = token.data.number_value;
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
            // Binary operator: pop 2, compute, push
            // ----------------------------------
            case BTOKEN_TYPE::OP: {
                registers.registers[1] = memory.st.pop_ret(); // RHS
                registers.registers[0] = memory.st.pop_ret(); // LHS

                switch (token.data.char_value) {
                    case '+': registers.registers[0].data.number_value += registers.registers[1].data.number_value; break;
                    case '-': registers.registers[0].data.number_value -= registers.registers[1].data.number_value; break;
                    case '*': registers.registers[0].data.number_value *= registers.registers[1].data.number_value; break;
                    case '/': registers.registers[0].data.number_value /= registers.registers[1].data.number_value; break;
                    case '<': registers.registers[0].data.number_value = registers.registers[0].data.number_value < registers.registers[1].data.number_value; break;
                    case '>': registers.registers[0].data.number_value = registers.registers[0].data.number_value > registers.registers[1].data.number_value; break;
                    case '=': registers.registers[0].data.number_value = registers.registers[0].data.number_value == registers.registers[1].data.number_value; break;
                    default:
                        throw_error("Unknown operator in bytecode: " + std::string(1, token.data.char_value));
                }

                memory.st.push(registers.registers[0]); // push result
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
}
