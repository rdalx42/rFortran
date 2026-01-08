
#ifndef LEXER_H
#define LEXER_H

#include <string> 
#include <vector>
#include <algorithm>
#include <fstream>
#include <cstdint>
#include <climits>
#include <cmath>

enum TOKEN_TYPE{
    IDENTIFIER,
    NUMBER,
    STRING,
    OPERATOR,
    PAREN,
    KEYWORD,
    NONE
};

enum class BTOKEN_TYPE: uint8_t {
    PUSH,
    LOAD,
    STORE,
    OP,
    NEG,
    NOT,
    LIST,
    LOADSTRING,
    GOTO,
    GOTO_IF_FALSE,
    LABEL,
    AND,
    OR
};

struct TOKEN{
    TOKEN_TYPE type;
    std::string value;
};

struct BTOKEN {
    BTOKEN_TYPE token_type;
    union Data {
        double number_value;
        unsigned char char_value;

        Data() {} // default constructor

        Data(double n) : number_value(n) {}
        Data(unsigned char c) : char_value(c) {}
    } data;

    // Constructors for convenience
    BTOKEN(BTOKEN_TYPE t, double n) : token_type(t), data(n) {}
    BTOKEN(BTOKEN_TYPE t, unsigned char c) : token_type(t), data(c) {}
};


const std::string skippables = " \n\t\r";
const std::vector<std::string>keywords = {"if","else","while","impl","var","end","else","program","do","list","concat","and","or"};
const std::vector<std::string>bytecode_keywords = {"PUSH","LOAD","STORE","OP","NEG","NOT","LIST","LOADSTRING","GOTO","GOTO_IF_FALSE","LABEL","AND","OR"};
const std::vector<std::string>expects_number_bytecode_keywords = {"PUSH","LOAD","STORE","LIST","LOADSTRING","GOTO","GOTO_IF_FALSE","LABEL"};
const std::vector<std::string>expects_char_bytecode_keywords = {"OP"};

struct LEXER{
    std::string src;
    
    std::vector<TOKEN>tokens;
    std::vector<BTOKEN>btokens;
    
    public: 
        void init(const std::string& source,bool is_bytecode=false);

    private:
        int pos=0;
        const char peek() const ;
        const char next() const;
        inline void advance() ;
        void lex_num();
        void lex_identifier();
        inline bool is_keyword(const std::string& val) const;  
        inline bool is_bytecode_keyword(const std::string& val) const;
        inline bool expects_number(const std::string& val) const;   
        inline bool expects_character(const std::string& val) const;
        double find_number();
        void lex(); 
        void lexb();
        void list();
        void listb();
        void lexb_identifier();
        void lex_string();
        
        const std::string token_type_to_string(const TOKEN_TYPE& type) const{
            switch(type){
                case TOKEN_TYPE::IDENTIFIER:
                    return "IDENTIFIER";
                case TOKEN_TYPE::NUMBER:
                    return "NUMBER";
                case TOKEN_TYPE::STRING:
                    return "STRING";
                case TOKEN_TYPE::OPERATOR:
                    return "OPERATOR";
                case TOKEN_TYPE::PAREN:
                    return "PAREN";
                case TOKEN_TYPE::KEYWORD:
                    return "KEYWORD";
                default:
                    return "UNKNOWN";
            }
        }

        const BTOKEN_TYPE string_to_bytecode_token_type(const std::string& type){
            if(type == "PUSH"){
                return BTOKEN_TYPE::PUSH;
            }else if(type == "LOAD"){
                return BTOKEN_TYPE::LOAD;
            }else if(type == "STORE"){
                return BTOKEN_TYPE::STORE;
            }else if(type == "AND"){
                return BTOKEN_TYPE::AND;
            }else if(type == "OR"){
                return BTOKEN_TYPE::OR;
            }else if(type == "OP"){
                return BTOKEN_TYPE::OP;
            }else if(type == "NEG"){
                return BTOKEN_TYPE::NEG;
            }else if(type == "NOT"){
                return BTOKEN_TYPE::NOT;
            }else if(type == "LIST"){
                return BTOKEN_TYPE::LIST;
            }else if(type == "LOADSTRING"){
                return BTOKEN_TYPE::LOADSTRING;
            }else if(type == "GOTO"){
                return BTOKEN_TYPE::GOTO;
            }else if(type == "GOTO_IF_FALSE"){
                return BTOKEN_TYPE::GOTO_IF_FALSE;
            }else if(type == "LABEL"){
                return BTOKEN_TYPE::LABEL;
            }
            else{
                throw std::runtime_error("Unknown bytecode token type: " + type);
            }
        }

        const std::string bytecode_token_type_to_string(const BTOKEN_TYPE& type) const{
            switch(type){
                case BTOKEN_TYPE::PUSH:
                    return "PUSH";
                case BTOKEN_TYPE::LOAD:
                    return "LOAD";
                case BTOKEN_TYPE::STORE:
                    return "STORE";
                case BTOKEN_TYPE::OP:
                    return "OP";
                case BTOKEN_TYPE::NEG:
                    return "NEG";
                case BTOKEN_TYPE::NOT:
                    return "NOT";
                case BTOKEN_TYPE::LIST:
                    return "LIST";
                case BTOKEN_TYPE::LOADSTRING:
                    return "LOADSTRING";
                case BTOKEN_TYPE::GOTO:
                    return "GOTO";
                case BTOKEN_TYPE::GOTO_IF_FALSE:
                    return "GOTO_IF_FALSE";
                case BTOKEN_TYPE::LABEL:
                    return "LABEL";
                case BTOKEN_TYPE::AND:
                    return "AND";
                case BTOKEN_TYPE::OR:
                    return "OR";
                default:
                    return "UNKNOWN";
            }
        }
};

#endif
