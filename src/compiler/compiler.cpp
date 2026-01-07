#include "compiler.h"
#include "../error/error.h"

void COMPILER::run() {

    while (ip < bytecode.size()) {
        const BTOKEN& token = bytecode[ip];

        switch (token.token_type) {

            // ----------------------------------
            // PUSH literal number onto stack
            // ----------------------------------
            case BTOKEN_TYPE::PUSH: {
                registers.registers[0].value_type = VALUE::VALUE_TYPE::NUMBER;
                registers.registers[0].data.number_value = token.data.value;
                memory.st.push(registers.registers[0]);
                ip++;
                break;
            }

            // ----------------------------------
            // LIST TOP from memory stack 
            // ----------------------------------

            case BTOKEN_TYPE::LIST:{
                this->memory.list_at(token.data.op_code);
                ip++;
                break;
            }

            // ----------------------------------
            // LOAD from memory into stack
            // ----------------------------------
            case BTOKEN_TYPE::LOAD: {
                uint16_t addr = token.data.value;
                registers.registers[0] = memory.memory[addr]; // load into register
                memory.st.push(registers.registers[0]);
                ip++;
                break;
            }

            // ----------------------------------
            // STORE from stack into memory
            // ----------------------------------
            case BTOKEN_TYPE::STORE: {
                uint16_t addr = token.data.value;
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

            default:
                throw_error("Unknown bytecode instruction");
        }
    }
}
