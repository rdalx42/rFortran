
#include "parser.h"
#include "../error/error.h"

// idea: functions dont have return, return last statement evaled.

void PARSER::parse_expression(){
    this->parse_term(); // first term
    while(this->idx < this->tokens.size() && this->tokens[this->idx].type == TOKEN_TYPE::OPERATOR && (this->tokens[this->idx].value == "+" || this->tokens[this->idx].value == "-")){
        std::string op = tokens[idx].value;
        this->advance();
        this->parse_term();
        this->bytecode += "OP " + op + "\n"; // emit operator bytecode
    }
}

void PARSER::parse_unary(){
    if(this->idx < this->tokens.size()
       && this->tokens[this->idx].type == TOKEN_TYPE::OPERATOR
       && (this->tokens[this->idx].value == "+"
           || this->tokens[this->idx].value == "-"
           || this->tokens[this->idx].value == "!")){

        std::string op = this->tokens[this->idx].value;
        this->advance();

        // allow chaining: !!x, -!x, etc.
        this->parse_unary();

        if(op == "-"){
            this->bytecode += "NEG\n";
        }
        else if(op == "!"){
            this->bytecode += "NOT\n";
        }
        // unary '+' | no-op
    }
    else{
        this->parse_factor();
    }
}


void PARSER::parse_term(){
    this->parse_unary();
    while(this->idx < this->tokens.size()
          && this->tokens[this->idx].type == TOKEN_TYPE::OPERATOR
          && (this->tokens[this->idx].value == "*" || this->tokens[this->idx].value == "/")){
        std::string op = tokens[idx].value;
        this->advance();
        this->parse_unary();
        this->bytecode += "OP " + op + "\n";
    }
}


void PARSER::parse_identifier(){
    const TOKEN& id_tok = this->tokens[idx];

    if(id_tok.type != TOKEN_TYPE::IDENTIFIER){
        throw_error("Expected identifier for assignment");
    }

    std::string var_name = id_tok.value;

   
    if(var_codification.find(var_name) == var_codification.end()){
        throw_error("Assignment to undeclared variable: " + var_name);
    }

    this->advance();

    if(idx >= tokens.size() || tokens[idx].type != TOKEN_TYPE::OPERATOR || tokens[idx].value != "="){
        throw_error("Expected '=' after variable name in assignment: " + var_name);
    }
    this->advance(); 
    this->parse_expression();

  
    unsigned short int var_code = var_codification[var_name];
    this->bytecode += "STORE " + std::to_string(var_code) + "\n";
}

void PARSER::parse_factor(){
    if(this->idx >= this->tokens.size()){
        throw_error("Unexpected end of input in factor");
    }

    TOKEN tok = this->tokens[this->idx];

    if(tok.type == TOKEN_TYPE::NUMBER){
        this->bytecode += "PUSH " + tok.value + "\n";
        this->advance();
    }
    else if(tok.type == TOKEN_TYPE::IDENTIFIER){
        if(this->var_codification.find(tok.value) == this->var_codification.end()){
            throw_error("Undefined variable: " + tok.value);
        }
        this->bytecode += "LOAD " + std::to_string(this->var_codification[tok.value]) + "\n";
        this->advance();
    }
    else if(tok.type == TOKEN_TYPE::PAREN && tok.value == "("){
        this->advance();
        this->parse_expression();
        if(this->idx >= this->tokens.size() || this->tokens[this->idx].type != TOKEN_TYPE::PAREN || this->tokens[this->idx].value != ")"){
            throw_error("Expected closing parenthesis");
        }
        this->advance();
    }else{
        throw_error("Unexpected token in factor: " + tok.value);
    }
}

void PARSER::parse_scope_start(){
    // Save current last var index as scope start
    int last_var_index = var_codification.empty() ? -1 : var_codification.size() - 1;
    scope_var_count.push(last_var_index);
    this->advance(); // now move past 'do'
}

void PARSER::parse_list(){

    this->advance();
    const TOKEN& list_id_tok = this->tokens[this->idx];
    if(list_id_tok.type != TOKEN_TYPE::IDENTIFIER){
        throw_error("Expected identifier after 'list'");
    }

    int list_id=-1;
    try {
        list_id = var_codification.at(list_id_tok.value);
    } catch (const std::out_of_range&) {
        throw_error("Variable not declared: " + list_id_tok.value);
    }

    this->bytecode += "LIST " + std::to_string(list_id) + "\n";

    this->advance(); 
}


void PARSER::parse_scope_end(){

    if(this->scope_var_count.empty()){
        throw_error("No open scope to end");
    }

    int vars_before_scope = this->scope_var_count.top();
    this->scope_var_count.pop();

    for(auto it = this->var_codification.begin(); it != this->var_codification.end(); ){
        if(it->second > vars_before_scope){
            it = this->var_codification.erase(it);
        } else {
            ++it;
        }
    }

    this->advance(); // now advance past 'end'
}

void PARSER::parse_var(){
  
    this->advance();
    const TOKEN& var_name_tok = this->tokens[this->idx];
    if(var_name_tok.type != TOKEN_TYPE::IDENTIFIER){
        throw_error("Expected variable name after 'var' keyword");
    }
    std::string var_name = var_name_tok.value;
    if(this->var_codification.find(var_name) != this->var_codification.end()){
        throw_error("Variable already declared: " + var_name);
    }
    unsigned short int var_code = var_codification.size();
    
    this->advance();

    if(this->idx>tokens.size()||this->tokens[this->idx].type != TOKEN_TYPE::OPERATOR || this->tokens[this->idx].value != "="){
        
        throw_error("Expected '=' after variable name in declaration");
    }
    
    this->advance();
    this->parse_expression();
    
    var_codification[var_name_tok.value] = var_code;

    this->bytecode += "STORE " + std::to_string(var_code) + "\n";
}

void PARSER::parse(){

    static unsigned int prog_start_tok_idx=-1;
    static unsigned int prog_end_tok_idx=-1;
   

    // init program

    bool prog_found = false;
    while(idx < tokens.size()){
        TOKEN tok = tokens[idx];

        if(tok.type == TOKEN_TYPE::KEYWORD && tok.value == "program"){
            // next token should be program name
            TOKEN prog_name_tok = peek();
            if(prog_name_tok.type == TOKEN_TYPE::NONE){
                throw_error("Expected program name after 'program'");
            }
            prog_name = prog_name_tok.value;

            // advance past 'program' keyword and program name
            advance(); // past 'program'
            advance(); // past name

            prog_start_tok_idx = idx; // program body starts here
            prog_found = true;
        }
        else if(tok.type == TOKEN_TYPE::KEYWORD && tok.value == "end"){
            advance(); // past 'end'
            if(idx < tokens.size() && tokens[idx].type == TOKEN_TYPE::KEYWORD && tokens[idx].value == "program"){
                prog_end_tok_idx = idx - 1; // last token of program body
                break;
            }
        }
        else{
            advance(); // advance in all other cases to prevent infinite loop
        }
    }

    if(prog_start_tok_idx == -1 || prog_end_tok_idx == -1){
        throw_error("Program must start with 'program <name>' and end with 'end program'");
    }else if(prog_end_tok_idx==prog_start_tok_idx){
        throw_error("Program body cannot be empty");
    }

    this->idx=prog_start_tok_idx;

    std::cout<<prog_start_tok_idx<<" "<<prog_end_tok_idx<<"\n";

    while(this->idx <= prog_end_tok_idx){
        const TOKEN& tok = tokens[idx];

        if(tok.type == TOKEN_TYPE::KEYWORD && tok.value == "var"){
            this->parse_var();
        } 
        else if(tok.type == TOKEN_TYPE::KEYWORD && tok.value == "list"){
            this->parse_list();
        }
        else if(tok.type == TOKEN_TYPE::IDENTIFIER){
            this->parse_identifier();
        }
        else if(tok.type == TOKEN_TYPE::KEYWORD && tok.value == "do"){
            this->parse_scope_start();
        }
        else if(tok.type == TOKEN_TYPE::KEYWORD && tok.value == "end" && this->peek().value!="program"){
            this->parse_scope_end();
        }
        else if(tok.type == TOKEN_TYPE::KEYWORD){
            this->advance(); // generic keyword, like 'then' or 'else'
        }
        else {
            throw_error("Unexpected token in program body: " + tok.value);
        }
    }


}

void PARSER::list() const{
    std::ios::sync_with_stdio(false);
    std::cout << "Program Name: " << this->prog_name << "\n\n";
    std::cout << "Bytecode:\n" << this->bytecode << "\n";
}