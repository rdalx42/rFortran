
#ifndef HASHER_H
#define HASHER_H

#include <vector>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <iostream>
#include "../../error/error.h"

struct STRING_HASHER{
    public:
        std::unordered_map<std::string, uint16_t>string_to_hash;
        std::vector<std::string>hashed_strings;
        void fill_hashed_strings(){
            hashed_strings.resize(string_to_hash.size());
            for(const auto& pair : string_to_hash){
                hashed_strings[pair.second] = pair.first;
            }
        }
        void list(){
            std::cout<<"[String Hasher] Hashed Strings:\n";
            for(size_t i=0;i<hashed_strings.size();i++){
                std::cout<<"Hash: "<<i<<" String: "<<hashed_strings[i]<<"\n";
            }
        }
};

struct GOTO_HASHER{
    public:
        std::unordered_map<uint16_t , uint16_t>label_to_address;
        std::vector<int>hashed_goto_positions; // index is label name, value is address in bytecode
        
        void add_label(uint16_t address){
            uint16_t label_name = label_to_address.size();
            if(label_to_address.find(label_name) != label_to_address.end()){
                throw_error("Duplicate label found in GOTO hasher: " + label_name);
            }
            label_to_address[label_name] = address;
        }

        void fill_hashed_goto_positions(){
            hashed_goto_positions.resize(label_to_address.size()+1);
            for(const auto& pair : label_to_address){
                hashed_goto_positions[pair.first] = pair.second;
            }
        }

        void set_label_address(uint16_t label_name, uint16_t address){
            if(label_to_address.find(label_name) == label_to_address.end()){
                throw_error("Label not found in GOTO hasher: " + std::to_string(label_name));
            }
           
            label_to_address[label_name] = address;
        }
        
        void list(){
            std::cout<<"[GOTO Hasher] Hashed GOTO Positions:\n";
            for(size_t i=0;i<hashed_goto_positions.size();i++){
                std::cout<<"Label: "<<i<<" Address: "<<hashed_goto_positions[i]<<"\n";
            }
        }
};

#endif 
