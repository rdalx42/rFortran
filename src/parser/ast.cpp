#include "ast.h"

inline bool is_operator(const TOKEN& tok, const std::vector<std::string>& ops) {
    return tok.type == TOKEN_TYPE::OPERATOR &&
           std::find(ops.begin(), ops.end(), tok.value) != ops.end();
}

inline bool is_keyword(const TOKEN& tok, const std::string& kw) {
    return tok.type == TOKEN_TYPE::KEYWORD && tok.value == kw;
}

// -------------------- Expressions --------------------
std::shared_ptr<EXPR> AST::parse_expression() {
    return parse_or();
}

std::shared_ptr<EXPR> AST::parse_or() {
    auto node = parse_and();
    while(idx < tokens.size() && is_keyword(tokens[idx], "or")) {
        auto bin = std::make_shared<EXPR>();
        bin->type = expression_type::BINARY;
        bin->binary_op = "or";
        idx++;
        bin->left = node;
        bin->right = parse_and();
        node = bin;
    }
    return node;
}

std::shared_ptr<EXPR> AST::parse_and() {
    auto node = parse_comparision();
    while(idx < tokens.size() && is_keyword(tokens[idx], "and")) {
        auto bin = std::make_shared<EXPR>();
        bin->type = expression_type::BINARY;
        bin->binary_op = "and";
        idx++;
        bin->left = node;
        bin->right = parse_comparision();
        node = bin;
    }
    return node;
}

std::shared_ptr<EXPR> AST::parse_comparision() {
    auto node = parse_additive();
    while(idx < tokens.size() && is_operator(tokens[idx], {"==", "!=", "<", "<=", ">", ">="})) {
        auto op = tokens[idx].value;
        idx++;
        auto right = parse_additive();
        auto bin = std::make_shared<EXPR>();
        bin->type = expression_type::BINARY;
        bin->binary_op = op;
        bin->left = node;
        bin->right = right;
        node = bin;
    }
    return node;
}

std::shared_ptr<EXPR> AST::parse_additive() {
    auto node = parse_term();
    while(idx < tokens.size() &&
          (is_operator(tokens[idx], {"+", "-"}) || (tokens[idx].type == TOKEN_TYPE::KEYWORD && tokens[idx].value == "concat"))) {

        auto op = tokens[idx].value;
        idx++;
        auto right = parse_term();

        if(op == "concat") {
            if(node->type != expression_type::LITERAL || right->type != expression_type::LITERAL ||
               node->literal_type != TOKEN_TYPE::STRING || right->literal_type != TOKEN_TYPE::STRING)
                throw_error("'concat' operator requires string literals only");
        }

        auto bin = std::make_shared<EXPR>();
        bin->type = expression_type::BINARY;
        bin->binary_op = op;
        bin->left = node;
        bin->right = right;
        node = bin;
    }
    return node;
}

std::shared_ptr<EXPR> AST::parse_term() {
    auto node = parse_unary();
    while(idx < tokens.size() && is_operator(tokens[idx], {"*", "/"})) {
        auto op = tokens[idx].value;
        idx++;
        auto right = parse_unary();
        auto bin = std::make_shared<EXPR>();
        bin->type = expression_type::BINARY;
        bin->binary_op = op;
        bin->left = node;
        bin->right = right;
        node = bin;
    }
    return node;
}

std::shared_ptr<EXPR> AST::parse_unary() {
    if(idx < tokens.size() && tokens[idx].type == TOKEN_TYPE::OPERATOR &&
       (tokens[idx].value == "+" || tokens[idx].value == "-" || tokens[idx].value == "!")) {

        auto node = std::make_shared<EXPR>();
        node->type = expression_type::UNARY;
        node->unary_op = tokens[idx].value;
        idx++;
        node->unary_expr = parse_unary();
        return node;
    }
    return parse_factor();
}

std::shared_ptr<EXPR> AST::parse_factor() {
    if(idx >= tokens.size()) throw_error("Unexpected end of input in factor");
    const auto& tok = tokens[idx];
    auto node = std::make_shared<EXPR>();

    if(tok.type == TOKEN_TYPE::NUMBER || tok.type == TOKEN_TYPE::STRING) {
        node->type = expression_type::LITERAL;
        node->value = tok.value;
        node->literal_type = tok.type;
        idx++;
    } 
    else if(tok.type == TOKEN_TYPE::IDENTIFIER) {
        node->type = expression_type::IDENTIFIER;
        node->name = tok.value;
        idx++;
    } 
    else if(tok.type == TOKEN_TYPE::PAREN && tok.value == "(") {
        idx++;
        node = parse_expression();
        if(idx >= tokens.size() || tokens[idx].type != TOKEN_TYPE::PAREN || tokens[idx].value != ")")
            throw_error("Expected ')' after expression");
        idx++;
    } 
    else {
        throw_error("Unexpected token in factor: " + tok.value);
    }
    return node;
}

// -------------------- Statement Parsing --------------------
std::shared_ptr<STMT> AST::parse_statement() {
    if(idx >= tokens.size()) return nullptr;
    const auto& tok = tokens[idx];

    if(tok.type == TOKEN_TYPE::KEYWORD) {
        if(tok.value == "var") return parse_var();
        else if(tok.value == "list") return parse_list();
        else if(tok.value == "if") return parse_if();
        else if(tok.value == "while") return parse_while();
        else if(tok.value == "do") return parse_block_stmt();
        else { idx++; return nullptr; }
    }
    else if(tok.type == TOKEN_TYPE::IDENTIFIER) {
        if(idx + 1 < tokens.size() && tokens[idx + 1].type == TOKEN_TYPE::OPERATOR && tokens[idx + 1].value == "=") {
            return parse_assignment();
        } else {
            throw_error("Unexpected identifier: " + tok.value);
        }
    }else { 
        idx++;
        return nullptr; 
    }
    return nullptr;
}

std::shared_ptr<STMT> AST::parse_var() {
    idx++;
    auto node = std::make_shared<STMT>();
    node->type = stmt_type::VAR_DECL;

    if(idx >= tokens.size() || tokens[idx].type != TOKEN_TYPE::IDENTIFIER)
        throw_error("Expected variable name after 'var'");
    node->var_name = tokens[idx].value;
    idx++;

    if(idx >= tokens.size() || !(tokens[idx].type == TOKEN_TYPE::OPERATOR && tokens[idx].value == "="))
        throw_error("Expected '=' in var declaration");
    idx++;

    node->init_expr = parse_expression();
    return node;
}

std::shared_ptr<STMT> AST::parse_assignment() {
    auto node = std::make_shared<STMT>();
    node->type = stmt_type::ASSIGNMENT;
    node->var_name = tokens[idx].value;
    idx++;

    if(idx >= tokens.size() || !(tokens[idx].type == TOKEN_TYPE::OPERATOR && tokens[idx].value == "="))
        throw_error("Expected '=' in assignment");
    idx++;

    node->assign_expr = parse_expression();
    return node;
}

std::shared_ptr<STMT> AST::parse_list() {
    idx++;
    auto node = std::make_shared<STMT>();
    node->type = stmt_type::LIST;

    if(idx >= tokens.size() || tokens[idx].type != TOKEN_TYPE::IDENTIFIER)
        throw_error("Expected identifier after 'list'");
    node->list_var_name = tokens[idx].value;
    idx++;
    return node;
}

std::shared_ptr<STMT> AST::parse_if() {
    idx++;
    auto node = std::make_shared<STMT>();
    node->type = stmt_type::IF;
    node->condition = parse_expression();

    if(idx >= tokens.size() || !(tokens[idx].type == TOKEN_TYPE::KEYWORD && tokens[idx].value == "do"))
        throw_error("Expected 'do' after if condition");
    idx++;

    node->then_block = parse_block();
    if(idx < tokens.size() && tokens[idx].type == TOKEN_TYPE::KEYWORD && tokens[idx].value == "else") {
        node->has_else=true;
        idx++;
        node->else_block = parse_block();
    }
    return node;
}

std::shared_ptr<STMT> AST::parse_while() {
    idx++;
    auto node = std::make_shared<STMT>();
    node->type = stmt_type::WHILE;
    node->condition = parse_expression();

    if(idx >= tokens.size() || !(tokens[idx].type == TOKEN_TYPE::KEYWORD && tokens[idx].value == "do"))
        throw_error("Expected 'do' after while condition");
    idx++;

    node->then_block = parse_block();
    return node;
}

std::shared_ptr<STMT> AST::parse_block_stmt() {
    idx++;
    auto node = std::make_shared<STMT>();
    node->type = stmt_type::BLOCK;
    node->then_block = parse_block();
    return node;
}

// -------------------- Block --------------------
std::vector<std::shared_ptr<STMT>> AST::parse_block() {
    std::vector<std::shared_ptr<STMT>> block;
    while(idx < tokens.size()) {
        if(tokens[idx].type == TOKEN_TYPE::KEYWORD && (tokens[idx].value == "end" || tokens[idx].value == "else"))
            break;
        block.push_back(parse_statement());
    }
    if(idx < tokens.size() && tokens[idx].type == TOKEN_TYPE::KEYWORD && tokens[idx].value == "end")
        idx++;
    return block;
}


void AST::list() {
    std::cout << "Program: " << this->program_name << "\n";
    for(const auto& stmt : statements) {
        list_stmt(stmt, 0);
    }
}

void AST::list_stmt(const std::shared_ptr<STMT>& stmt, int indent) {
    if(!stmt) return;
    std::string pad(indent * 2, ' ');

    switch(stmt->type) {
        case stmt_type::VAR_DECL:
            std::cout << pad << "VarDecl: " << stmt->var_name << " = ";
            list_expr(stmt->init_expr, 0);
            std::cout << "\n";
            break;
        case stmt_type::ASSIGNMENT:
            std::cout << pad << "Assignment: " << stmt->var_name << " = ";
            list_expr(stmt->assign_expr, 0);
            std::cout << "\n";
            break;
        case stmt_type::LIST:
            std::cout << pad << "List: " << stmt->list_var_name << "\n";
            break;
        case stmt_type::IF:
            std::cout << pad << "If: ";
            list_expr(stmt->condition, 0);
            std::cout << "\n";
            std::cout << pad << "Then:\n";
            for(const auto& s : stmt->then_block) list_stmt(s, indent + 1);
            if(!stmt->else_block.empty()) {
                std::cout << pad << "Else:\n";
                for(const auto& s : stmt->else_block) list_stmt(s, indent + 1);
            }
            break;
        case stmt_type::WHILE:
            std::cout << pad << "While: ";
            list_expr(stmt->condition, 0);
            std::cout << "\n";
            for(const auto& s : stmt->then_block) list_stmt(s, indent + 1);
            break;
        case stmt_type::BLOCK:
            std::cout << pad << "Block:\n";
            for(const auto& s : stmt->then_block) list_stmt(s, indent + 1);
            break;
    }
}

void AST::list_expr(const std::shared_ptr<EXPR>& expr, int indent) {
    if(!expr) return;
    std::string pad(indent * 2, ' ');

    switch(expr->type) {
        case expression_type::LITERAL:
            std::cout << pad << "Literal(" << expr->value << ")";
            break;
        case expression_type::IDENTIFIER:
            std::cout << pad << "Identifier(" << expr->name << ")";
            break;
        case expression_type::UNARY:
            std::cout << pad << "Unary(" << expr->unary_op << " ";
            list_expr(expr->unary_expr, 0);
            std::cout << ")";
            break;
        case expression_type::BINARY:
            std::cout << pad << "Binary(";
            list_expr(expr->left, 0);
            std::cout << " " << expr->binary_op << " ";
            list_expr(expr->right, 0);
            std::cout << ")";
            break;
    }
}

// -------------------- Parse top-level --------------------
void AST::parse() {
    
    if(idx >= tokens.size() || !is_keyword(tokens[idx], "program")){
        throw_error("Expected 'program' at the beginning");
    }

    idx++;

    if(idx >= tokens.size() || tokens[idx].type != TOKEN_TYPE::IDENTIFIER){
        throw_error("Expected program name after 'program' keyword");
    }

    const int prev_idx=idx;

    while(idx<tokens.size()-1){
        
        if(tokens[idx].value == "program" && tokens[idx].type == TOKEN_TYPE::KEYWORD){
            throw_error("Only one program is allowed per file!");
        }
        idx++;
    }

    idx=prev_idx;

    this->program_name = tokens[idx].value;
    idx++;

    while(idx < tokens.size()) {
        if(is_keyword(tokens[idx], "end")) {
            idx++;
            if(idx < tokens.size() && is_keyword(tokens[idx], "program")) {
                idx++;
                if(idx < tokens.size() && tokens[idx].type == TOKEN_TYPE::IDENTIFIER) {
                    throw_error("Multiple programs detected: '" + tokens[idx].value + "'");
                } 
            }else{
                throw_error("Expected 'end program' sequence");
            }
            break; // End of program
        }

        auto stmt = parse_statement();
        if(stmt) statements.push_back(stmt);
    }

   
}

// CODEGEN 

void AST::parse_scope_start(){
    
    int last_var_index = var_codification.empty() ? -1 : var_codification.size() - 1;
    scope_var_count.push(last_var_index);
    
}

void AST::parse_scope_end(){

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
}

void AST::codegen_expr(std::shared_ptr<EXPR>& expr){
    if(!expr){return;}
    
    switch (expr->type){
        case expression_type::LITERAL:
        {
            if(expr->literal_type == TOKEN_TYPE::STRING){
                
                // alloc new string in string pool

                if(this->string_hasher.string_to_hash.find(expr->value)==this->string_hasher.string_to_hash.end()){
                    uint16_t string_hash_id = this->string_hasher.string_to_hash.size();
                    this->string_hasher.string_to_hash[expr->value] = string_hash_id;
                    this->bytecode+="LOADSTRING "+std::to_string(string_hash_id) + "\n";
                }else{
                    uint16_t string_hash_id = this->string_hasher.string_to_hash[expr->value];
                    this->bytecode+="LOADSTRING " + std::to_string(string_hash_id) + "\n";
                }
            }else{
                // simply push number
                this->bytecode+="PUSH " + expr->value + "\n";
            }

            break;
        }

        case expression_type::UNARY:{

            this->codegen_expr(expr->unary_expr);

            switch(expr->unary_op[0]){
                case '+':
                    break;
                case '!':
                    this->bytecode += "NOT\n";
                    break;
                case '-':
                    this->bytecode+="NEG\n";
                    break;
                default:
                    throw_error(std::string("Invalid unary op: '" + expr->unary_op[0] + '\''));
                    break;
            }

            break;
        }

        case expression_type::IDENTIFIER: {
            const std::string& var_name = expr->name;

            if(this->var_codification.find(var_name) == this->var_codification.end()) {
                throw_error("Invalid variable of name: " + var_name);
                break;
            }

            uint16_t var_code = var_codification[var_name];
            this->bytecode += "LOAD " + std::to_string(var_code) + "\n";
            break;
        }


        case expression_type::BINARY:{

            const std::string& op = expr->binary_op;

            if(!(op == "concat")){
                codegen_expr(expr->left);
                codegen_expr(expr->right);
            }

            if(op == "+"){
                this->bytecode+="OP +\n";
            }else if(op == "-"){
                this->bytecode+="OP -\n";
            }else if(op == "*"){
                this->bytecode+="OP *\n";
            }else if(op == "/"){
                this->bytecode+="OP /\n";
            }else if(op == "=="){
                this->bytecode+="OP =\n";
            }else if(op=="!="){
                this->bytecode+="OP ~\n";
            }else if(op == "<="){
                this->bytecode+="OP [\n";
            }else if(op == ">="){
                this->bytecode+="OP ]\n";
            }else if(op == ">"){
                this->bytecode+="OP >\n";
            }else if(op == "<"){
                this->bytecode+="OP <\n";
            }else if(op == "and"){
                this->bytecode+="AND\n";
            }else if(op == "or"){
                this->bytecode+="OR\n";
            }else if(op == "concat"){
                
                const std::string &Lstring_value = expr->left->value;
                const std::string &Rstring_value = expr->right->value;

                const std::string concatenated = Lstring_value + Rstring_value;

                if(this->string_hasher.string_to_hash.find(concatenated)!=this->string_hasher.string_to_hash.end()){
                    this->bytecode+="LOADSTRING " + std::to_string(this->string_hasher.string_to_hash[concatenated]) + "\n";
                }else{
                    // alloc new string id.
                    uint16_t string_hash_id = this->string_hasher.string_to_hash.size();
                    this->string_hasher.string_to_hash[concatenated] = string_hash_id;
                    this->bytecode+="LOADSTRING "+std::to_string(string_hash_id) + "\n";
                }
            }

            break;
        }

        default:
            throw_error("Invalid expression type detected!");
            break;
    }
}

void AST::codegen(std::shared_ptr<STMT>&stmt){
    
    if(!stmt){ return; }

    switch(stmt->type){
        case stmt_type::VAR_DECL:{

            const std::string& var_name = stmt->var_name;
            if(this->var_codification.find(var_name) != this->var_codification.end()){
                throw_error("Variable already declared: " + var_name);
            }

            uint16_t var_code = var_codification.size();
            
            this->codegen_expr(stmt->init_expr);
            this->var_codification[var_name] = var_code;
            this->bytecode += "STORE " + std::to_string(var_code) + "\n";

            break;
        }

        case stmt_type::ASSIGNMENT:{

            const std::string& var_name = stmt->var_name;

            if(this->var_codification.find(var_name) == this->var_codification.end()){
                throw_error("Variable of name: " + var_name + " hasn't been declared");
            }

            uint16_t var_code = var_codification[var_name];

            this->codegen_expr(stmt->assign_expr);
            this->bytecode += "STORE " + std::to_string(var_code) + "\n";

            break;
        }

        case stmt_type::BLOCK:{
            
            this->parse_scope_start();
            for(auto& Stmt : stmt->then_block){
                codegen(Stmt);
            }

            if(this->scope_var_count.empty()){
                throw_error("No open scope to end");
            }

            this->parse_scope_end();

            break;
        }

        case stmt_type::LIST:{
            
            const std::string& var_name = stmt->list_var_name;

            if(this->var_codification.find(var_name) == this->var_codification.end()){
                throw_error("Invalid variable of name: " + var_name);
            }

            this->bytecode+="LIST " + std::to_string(this->var_codification[var_name]) + "\n";

            break;
        }

        case stmt_type::WHILE:{

            uint16_t start_label_id = this->goto_hasher.label_to_address.size();
            this->goto_hasher.add_label(0); // temp address
            uint16_t end_label_id = this->goto_hasher.label_to_address.size();
            this->goto_hasher.add_label(0); // temp address

            this->bytecode+="LABEL "+std::to_string(start_label_id)+"\n";
            this->codegen_expr(stmt->condition);
            this->bytecode+="GOTO_IF_FALSE " + std::to_string(end_label_id) + "\n";

            this->parse_scope_start();

            for(auto& Stmt : stmt->then_block) {
                this->codegen(Stmt);
            }

            this->parse_scope_end();

            this->bytecode += "GOTO " + std::to_string(start_label_id) + "\n";
            this->bytecode += "LABEL " + std::to_string(end_label_id) + "\n";

            break;
        }

        case stmt_type::IF:{

            this->codegen_expr(stmt->condition);
            
            uint16_t end_label_id = this->goto_hasher.label_to_address.size();
            this->goto_hasher.add_label(0); // temp address
            
            if(!stmt->has_else){
                this->bytecode+="GOTO_IF_FALSE " + std::to_string(end_label_id) + "\n";

                this->parse_scope_start();

                for(auto& Stmt:stmt->then_block){
                    codegen(Stmt);
                } 

                this->parse_scope_end();

                this->bytecode+="LABEL " + std::to_string(end_label_id) + "\n";
            }else{

                uint16_t else_label_id = this->goto_hasher.label_to_address.size();
                this->goto_hasher.add_label(0); // placeholder

                this->bytecode += "GOTO_IF_FALSE " + std::to_string(else_label_id) + "\n";

                this->parse_scope_start();
                for (auto& s : stmt->then_block) {
                    codegen(s);
                }
                this->parse_scope_end();

                this->bytecode += "GOTO " + std::to_string(end_label_id) + "\n";
                this->bytecode += "LABEL " + std::to_string(else_label_id) + "\n";

                // --- ELSE BLOCK ---
                this->parse_scope_start();
                for (auto& s : stmt->else_block) {
                    codegen(s);
                }
                this->parse_scope_end();

                // End label
                this->bytecode += "LABEL " + std::to_string(end_label_id) + "\n";
            }

            break;
        }
    }    
}

void AST::init_codegen(){
    for(auto &stmt:this->statements){
        this->codegen(stmt);
    }
}

void AST::list_bytecode(){
    std::cout<<this->bytecode;
}
