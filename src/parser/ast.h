#include "../lexer/lexer.h"
#include "../error/error.h"
#include "../runtime/memory/hasher.h"
#include <unordered_map>
#include <stack>
#include <sstream>
#include <cstdint>
#include <memory>
#include <functional>

#ifndef AST_H
#define AST_H

// -------------------- Expression --------------------
enum class expression_type : uint8_t {
    LITERAL,    // number or string literal
    IDENTIFIER, // variable
    UNARY,      // unary op
    BINARY      // binary op
};

struct EXPR {
    expression_type type;

    TOKEN_TYPE literal_type; 
    std::string value;              // literal value
    std::string name;               // identifier name

    std::string unary_op;           
    std::shared_ptr<EXPR> unary_expr;

    std::string binary_op;
    std::shared_ptr<EXPR> left;
    std::shared_ptr<EXPR> right;
};

// -------------------- Statements --------------------
enum class stmt_type : uint8_t {
    VAR_DECL,   // var declaration
    ASSIGNMENT, // variable assignment
    LIST,       // list statement
    IF,         // if statement
    WHILE,      // while loop
    BLOCK       // scope block
};

struct STMT {
    stmt_type type;

    std::string var_name;                 // var decl or assignment
    std::shared_ptr<EXPR> init_expr;      // var initializer
    std::shared_ptr<EXPR> assign_expr;    // assignment expression
    std::string list_var_name;            // list statement variable

    std::shared_ptr<EXPR> condition;      // if or while condition
    std::vector<std::shared_ptr<STMT>> then_block; // if/while/block body
    std::vector<std::shared_ptr<STMT>> else_block; // optional else

    bool has_else=false;
};

struct AST {
    
    std::string program_name;
    std::vector<std::shared_ptr<STMT>> statements; // top-level statements
    
    STRING_HASHER string_hasher;
    GOTO_HASHER goto_hasher;
    
    std::vector<TOKEN>tokens;
    std::string bytecode="";
    std::string prog_name="";
    std::stack<short int>scope_var_count;
    std::unordered_map<std::string,unsigned short int>var_codification;
    int idx=0;

    public:
        void init(const std::vector<TOKEN>& tokens){
            this->tokens = tokens;
            this->parse();
            this->list();
            this->init_codegen();
            this->list_bytecode();
            string_hasher.fill_hashed_strings();
            
            // string_hasher.fill_hashed_strings();
            // string_hasher.list();
        }

    private:
        void parse();
        
        void list();
        void list_stmt(const std::shared_ptr<STMT>& stmt, int indent);
        void list_expr(const std::shared_ptr<EXPR>& expr, int indent);

        // -------------------- Expression Parsing --------------------
        std::shared_ptr<EXPR> parse_expression();
        std::shared_ptr<EXPR> parse_or();
        std::shared_ptr<EXPR> parse_and();
        std::shared_ptr<EXPR> parse_comparision();
        std::shared_ptr<EXPR> parse_additive();
        std::shared_ptr<EXPR> parse_term();
        std::shared_ptr<EXPR> parse_unary();
        std::shared_ptr<EXPR> parse_factor();

        // -------------------- Statement Parsing --------------------
        std::shared_ptr<STMT> parse_statement();
        std::shared_ptr<STMT> parse_var();
        std::shared_ptr<STMT> parse_assignment();
        std::shared_ptr<STMT> parse_if();
        std::shared_ptr<STMT> parse_while();
        std::vector<std::shared_ptr<STMT>> parse_block();
        std::shared_ptr<STMT>parse_list();
        std::shared_ptr<STMT>parse_block_stmt();

        // -------------------- Scope Helpers --------------------
        
        void parse_scope_start();
        void parse_scope_end();

        // CODEGEN 

        void init_codegen(); // code generation start point
        void codegen(std::shared_ptr<STMT>&stmt); // generate bytecode and implement all optimizatiosns over here.
        void codegen_expr( std::shared_ptr<EXPR>&expr); // generate bytecode and implement all optimizatiosns over here.
        void list_bytecode();
};

#endif 
