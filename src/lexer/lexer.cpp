
#include "lexer.h"
#include "../error/error.h"

void LEXER::init(const std::string& source_filename, bool is_bytecode){
    this->pos=0;
    this->src+='\n';
    if(!is_bytecode){
        std::ifstream file(source_filename);
        if(!file.is_open()){
            throw_error("Could not open source file: " + source_filename);
        }

        std::string line;
        while(std::getline(file,line)){
            this->src+=line+'\n';
        }

        file.close();
    }else{
        this->src = source_filename;
    }

    if(!is_bytecode){
        this->lex();
        this->list(); 
    }else{
        this->lexb();
        this->listb();
    }
    
}

const char LEXER::peek() const {
    if (this->pos >= src.size()) {return '\0';}
    return this->src[pos];
}

inline void LEXER::advance() {
    this->pos++;
}  

inline bool LEXER::is_keyword(const std::string& val) const{
    return std::find(keywords.begin(),keywords.end(),val)!=keywords.end();
}

inline bool LEXER::is_bytecode_keyword(const std::string& val) const{
    return std::find(bytecode_keywords.begin(),bytecode_keywords.end(),val)!=bytecode_keywords.end();
}

inline bool LEXER::expects_number(const std::string& val) const{
    return std::find(expects_number_bytecode_keywords.begin(),expects_number_bytecode_keywords.end(),val)!=expects_number_bytecode_keywords.end();
}

void LEXER::lex_num(){
    std::string value = "";
    while(isdigit(this->peek())|| this->peek() == '_' || this->peek() == '.'){
        value+=this->peek();
        this->advance();
    }
    this->tokens.push_back({TOKEN_TYPE::NUMBER,value});
}

void LEXER::lex_identifier(){
    std::string value = "";
    while(isalnum(this->peek()) || this->peek() == '_'){
        value+=this->peek();
        this->advance();
    }
    if(this->is_keyword(value)){
        this->tokens.push_back({TOKEN_TYPE::KEYWORD,value});
    }else{
        this->tokens.push_back({TOKEN_TYPE::IDENTIFIER,value});
    }
}

int LEXER::find_number(){
    std::string value = "";
    while(isdigit(this->peek()) || this->peek() == '.'){
        value+=this->peek();
        this->advance();
    }
    return std::stoi(value);
}

inline bool LEXER::expects_character(const std::string& val) const{
    return std::find(expects_char_bytecode_keywords.begin(),expects_char_bytecode_keywords.end(),val)!=expects_char_bytecode_keywords.end();
}

void LEXER::lexb_identifier(){
    std::string value = "";
    while(isalnum(this->peek()) || this->peek() == '_'){
        value+=this->peek();
        this->advance();
    }

    
    if (this->is_bytecode_keyword(value) && this->expects_number(value)) {
        
        this->advance();
        int nr_found = this->find_number();
        if(nr_found<=UINT8_MAX){
            this->btokens.push_back({ string_to_bytecode_token_type(value), static_cast<uint8_t>(nr_found) });
        }else if(nr_found<=UINT16_MAX){
            this->btokens.push_back({ string_to_bytecode_token_type(value), static_cast<uint16_t>(nr_found) });
        }else{
            throw_error("Number too large for bytecode token: " + std::to_string(nr_found));
        }
    }else if(!this->is_bytecode_keyword(value)){
        throw_error("Unexpected bytecode token: " + value);
    }else if(this->expects_character(value)){ // one single character
        this->advance();
        unsigned char char_found = this->src[this->pos];
        this->btokens.push_back({ string_to_bytecode_token_type(value), char_found });
        this->advance();
    }else{
        this->btokens.push_back({ string_to_bytecode_token_type(value), 0 });
        
    }
}

void LEXER::lex(){
    while(this->peek()!='\0'){
        if(skippables.find(this->peek())!=std::string::npos){
            this->advance();
        }
        else if(isdigit(this->peek())){
            this->lex_num();
        }
        else if(isalpha(this->peek()) || this->peek() == '_'){
            this->lex_identifier();
        }
        else{
            switch(this->peek()){
                case '+':
                case '-':
                case '*':
                case '/':
                case '=':
                case '<':
                case '>':
                case '!':
                    this->tokens.push_back({TOKEN_TYPE::OPERATOR,std::string(1,this->peek())});
                    this->advance();
                    break;
                case '(':
                case ')':
                    this->tokens.push_back({TOKEN_TYPE::PAREN,std::string(1,this->peek())});
                    this->advance();
                    break;
                default:
                    throw_error("Unexpected character: " + std::string(1,this->peek()));
                    break;
            }
        }
    }
}

void LEXER::lexb(){
    while(this->peek()!='\0'){
        if(skippables.find(this->peek())!=std::string::npos){
            this->advance();
        }
        else if(isalpha(this->peek())){
            this->lexb_identifier();
        }else{
            throw_error("Unexpected character in bytecode: " + std::string(1,this->peek()));
        }
    }
}

void LEXER::list(){
    
    std::ios::sync_with_stdio(false);

    for(const auto& token : this->tokens){
        std::cout << "Type: " << token_type_to_string(token.type) << " Value: " << token.value << "\n";
    }
}

void LEXER::listb(){

    std::ios::sync_with_stdio(false);
    int I=0;

    for(const auto& btoken : this->btokens){
        std::cout << "{Type: " << this->bytecode_token_type_to_string(btoken.token_type) ;
        if(this->expects_character(this->bytecode_token_type_to_string(btoken.token_type)) ){
            std::cout << " Value: " << static_cast<char>(btoken.data.char_value);
        }else if(this->expects_number(this->bytecode_token_type_to_string(btoken.token_type)) ){
            if(!btoken.data.value){
                std::cout << " Value: " << static_cast<unsigned short int>(btoken.data.op_code);
            }else{
                std::cout << " Value: " << static_cast<unsigned int>(btoken.data.op_code);
            }
        }

        if(I+1==this->btokens.size()){       
            std::cout << "}\n";
        }else{
            std::cout << "},\n";
        }

        I++;
        
    }
}