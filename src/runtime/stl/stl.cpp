#include "stl.h"
#include "../../compiler/compiler.h"
#include <iostream>

void STL::init(COMPILER* compiler) {
    this->comp = compiler;
    builtins[0] = [this](bool standalone){ this->List(standalone); };
    builtins[1] = [this](bool standalone){ this->Time(standalone); };
    builtins[2] = [this](bool standalone){ this->RandRange(standalone); };
    builtins[3] = [this](bool standalone){this->GetChar(standalone);};
    builtins[4] = [this](bool standalone){this->GetType(standalone);};
}

void STL::List(bool standalone) {
    if(this->comp->memory.param_stack.psp == 0){
        throw_error("List function expects at least 1 parameter!");
    }

    while(!this->comp->memory.param_stack.empty()){
        const VALUE& val = this->comp->memory.param_stack.front();

        switch(val.value_type){
            case VALUE_TYPE::NUMBER:
                std::cout <<  val.data.number_value;
                break;
            case VALUE_TYPE::STRING:
                std::cout << this->comp->memory.string_hasher->hashed_strings[val.data.string_pointer_to_string_hash_array];
                break;
            case VALUE_TYPE::NONE:
                std::cout << "null";
                break;
            case VALUE_TYPE::ENUM_OBJECT:
                std::cout << std::to_string(val.data.enum_data.value_id);
                break;
            case VALUE_TYPE::ARRAY:{
                std::cout << "[ ";
                int idx = 0;
                while(idx < UINT8_MAX){
                    const VALUE& elem = this->comp->memory.array_memory[val.data.array_pointer][idx];
                    if(elem.value_type == VALUE_TYPE::NONE) break;
                    switch(elem.value_type){
                        case VALUE_TYPE::NUMBER: std::cout << elem.data.number_value; break;
                        case VALUE_TYPE::STRING: std::cout << this->comp->memory.string_hasher->hashed_strings[elem.data.string_pointer_to_string_hash_array]; break;
                        default: std::cout << "UNKNOWN"; break;
                    }
                    std::cout << " ";
                    idx++;
                }
                std::cout << "]";
                break;
            }
            default:
                std::cout << "UNKNOWN ";
                break;
        }

        this->comp->memory.param_stack.pop_front();
    }
    std::cout << "\n";

    if(!standalone){
        this->comp->registers.registers[0].value_type = VALUE_TYPE::NONE;
    }
}

void STL::RandRange(bool standalone) {

    if (this->comp->memory.param_stack.psp != 2) {
        throw_error("RandRange function expects 2 parameters!");
    }

    if (standalone) return;


    const VALUE& min_val = this->comp->memory.param_stack.front();
    this->comp->memory.param_stack.pop_front();
    const VALUE& max_val = this->comp->memory.param_stack.front();
    this->comp->memory.param_stack.pop_front();

    if (min_val.value_type != VALUE_TYPE::NUMBER || max_val.value_type != VALUE_TYPE::NUMBER) {
        throw_error("RandRange function expects number parameters!");
    }

    double min = min_val.data.number_value;
    double max = max_val.data.number_value;

    if (min > max) std::swap(min, max); 

    static std::random_device rd;  
    static std::mt19937 gen(rd()); 
    std::uniform_real_distribution<double> dist(min, max);

    double result = dist(gen);

    number_registers[0] = result;

    VALUE val;
    val.value_type = VALUE_TYPE::NUMBER;
    val.data.number_value = result;

    this->comp->registers.registers[0] = val;
    this->comp->memory.st.push(val);
}

void STL::Time(bool standalone) {

    if(this->comp->memory.param_stack.psp != 0){
        throw_error("Time function doesn't expect params!");
    }

    if (standalone) return;

    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);

    std::tm local_tm = *std::localtime(&now_time_t);

    double ms_since_midnight = (local_tm.tm_hour * 3600 + local_tm.tm_min * 60 + local_tm.tm_sec) * 1000;

    auto duration_since_second = now.time_since_epoch() - std::chrono::seconds(now_time_t);
    ms_since_midnight += std::chrono::duration_cast<std::chrono::milliseconds>(duration_since_second).count();

    number_registers[0] = ms_since_midnight;

    this->comp->registers.registers[0].value_type = VALUE_TYPE::NUMBER;
    this->comp->registers.registers[0].data.number_value = number_registers[0];

    this->comp->memory.st.push(this->comp->registers.registers[0]);
}

void STL::GetChar(bool standalone)
{
    if (this->comp->memory.param_stack.psp != 0) {
        throw_error("GetChar function doesn't expect params!");
    }

    char c;
    std::cin>>c;

    if (standalone) {
        return;
    }

    if (c == '\n' || c == '\r' || c == '\t' || c == ' ') {
        return;
    }

    std::string key(1, c);
    
    auto* hasher = this->comp->memory.string_hasher;
    auto it = hasher->string_to_hash.find(key);

    if (it == hasher->string_to_hash.end()) {
        std::cout << "invalid char\n";
        return;
    }

    size_t id = it->second;

    if (id >= hasher->hashed_strings.size()) {
        std::cout << "invalid char id (out of bounds)\n";
        return;
    }

    this->comp->registers.registers[0].value_type = VALUE_TYPE::STRING;
    this->comp->registers.registers[0].data.string_pointer_to_string_hash_array = id;
    this->comp->memory.st.push(this->comp->registers.registers[0]);
}

void STL::GetType(bool standalone){

    if (this->comp->memory.param_stack.psp != 1) {
        throw_error("GetType function expects only one param");
    }

    if(standalone){
        return;
    }

    this->comp->registers.registers[0] = this->comp->memory.param_stack.front();
    this->comp->memory.param_stack.pop_front();

    switch(this->comp->registers.registers[0].value_type){
        case VALUE_TYPE::STRING:
            this->comp->registers.registers[0].value_type = VALUE_TYPE::STRING;
            this->comp->registers.registers[0].data.string_pointer_to_string_hash_array = this->comp->memory.string_hasher->string_to_hash["string"];
            break;
        case VALUE_TYPE::NUMBER:
            this->comp->registers.registers[0].value_type = VALUE_TYPE::STRING;
            this->comp->registers.registers[0].data.string_pointer_to_string_hash_array = this->comp->memory.string_hasher->string_to_hash["number"];
            break;
        case VALUE_TYPE::ENUM_OBJECT:
            this->comp->registers.registers[0].value_type = VALUE_TYPE::STRING;
            this->comp->registers.registers[0].data.string_pointer_to_string_hash_array = this->comp->memory.string_hasher->string_to_hash["enum_object"];
            break;
        case VALUE_TYPE::ARRAY:
            this->comp->registers.registers[0].value_type = VALUE_TYPE::STRING;
            this->comp->registers.registers[0].data.string_pointer_to_string_hash_array = this->comp->memory.string_hasher->string_to_hash["array"];
            break;
        case VALUE_TYPE::NONE:
            this->comp->registers.registers[0].value_type = VALUE_TYPE::STRING;
            this->comp->registers.registers[0].data.string_pointer_to_string_hash_array = this->comp->memory.string_hasher->string_to_hash["none"];
            break;
        default:
            throw_error("Invalid value type in GetType builtin!");
            break;
    }

    this->comp->memory.st.push(this->comp->registers.registers[0]);
}
