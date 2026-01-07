
#ifndef PARSER_H
#define PARSER_H

#include "../lexer/lexer.h"
#include "../error/error.h"
#include <unordered_map>
#include <stack>

// turn tokens into bytecode

struct PARSER{
    std::vector<TOKEN>tokens;
    std::string bytecode;
    std::string prog_name="";
    std::stack<unsigned short int>scope_var_count;
    std::unordered_map<std::string,unsigned short int>var_codification;
    int idx=0;
    public:
        void init(const std::vector<TOKEN>& tokens){
            this->tokens = tokens;
            this->parse();
            this->list();
        }
    private:
        void parse();
        
        inline TOKEN peek() const {
            if (idx + 1 >= tokens.size()) {
                return TOKEN{TOKEN_TYPE::NONE, "none"}; // safe: returning a copy
            } else {
                return tokens[idx + 1]; // returns a copy
            }
        }

        inline void advance(){
            this->idx++;
        }
       
        void parse_term();
        void parse_factor();
        void parse_var();
        void parse_identifier();
        void parse_expression();

        void parse_scope_start();
        void parse_scope_end();
        void parse_unary();
        void parse_list();
        
        void list() const;
};

#endif 