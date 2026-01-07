
#ifndef STRING_HASHER_H
#define STRING_HASHER_H

#include <vector>
#include <cstdint>
#include <string>
#include <unordered_map>

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

#endif 
