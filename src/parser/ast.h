#include "../lexer/lexer.h"
#include "../error/error.h"
#include "../runtime/memory/hasher.h"
#include <unordered_map>
#include <stack>
#include <sstream>
#include <cstdint>
#include <memory>
#include <functional>
#include <climits>

#ifndef AST_H
#define AST_H

// -------------------- Expression --------------------
enum class expression_type : uint8_t {
    LITERAL,    // number or string literal
    IDENTIFIER, // variable
    UNARY,      // unary op
    BINARY,      // binary op
    ARRAY_LITERAL,
    ARRAY_ACCESS,
    ENUM_LITERAL,
    ENUM_ACCESS,
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

    // --- ENUM SUPPORT --- 
    std::vector<std::string>enum_elements;
    std::string enum_name;
    std::string enum_value;

    // --- ARRAY SUPPORT ---
    std::vector<std::shared_ptr<EXPR>> array_elements; // for [a, b, c]
    std::shared_ptr<EXPR> array_index;                 // for arr[i]
    std::string array_name;                         // variable holding the array
};

// -------------------- Statements --------------------
enum class stmt_type : uint8_t {
    VAR_DECL,   // var declaration
    ASSIGNMENT, // variable assignment
    LIST,       // list statement
    IF,         // if statement
    WHILE,      // while loop
    BLOCK,       // scope block
    ENUM,
};

struct STMT {
    stmt_type type;

    std::string var_name;                 // var decl or assignment
    std::shared_ptr<EXPR>array_assign_expr; // array assignment expression x[expr]
    std::shared_ptr<EXPR> init_expr;      // var initializer
    std::shared_ptr<EXPR> assign_expr;    // assignment expression
    std::string list_var_name;            // list statement variable

    std::shared_ptr<EXPR> condition;      // if or while condition
    std::vector<std::shared_ptr<STMT>> then_block; // if/while/block body
    std::vector<std::shared_ptr<STMT>> else_block; // optional else
    std::string enum_name ; // enum decl
    std::shared_ptr<EXPR> enum_body;

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
    std::unordered_map<std::string,uint8_t>array_codification;
    std::unordered_map<std::string,bool>variables_in_declaration_proccess;  /* 
    the reason why we do this is simple, array evaluation needs direct variable name but we don't register the var name
    before we parse the expression, which leads to an error, therefore we pre register it and than we erase it
    */
          
    std::unordered_map<int,std::vector<int>>enum_map; // enum idx=> enum values
    std::unordered_map<std::string,std::vector<std::string>>enum_value_to_enums; // enum holder -> enum clasifications
    std::unordered_map<std::string,uint8_t>enum_name_to_uint8;
 //   VALUE em[MAX_ENUM][MAX_ENUM];
    int idx=0;

    public:
        void init(const std::vector<TOKEN>& tokens){
            this->tokens = tokens;
            this->parse();
            this->check_array_rules();
            this->list();
            this->init_codegen();
            this->list_bytecode();
            string_hasher.fill_hashed_strings();
            
            // string_hasher.fill_hashed_strings();
            // string_hasher.list();
        }

    private:
        void parse();
        
       // void init_external_mem_objects();
        void list();
        void list_stmt(const std::shared_ptr<STMT>& stmt, int indent);
        void list_expr(const std::shared_ptr<EXPR>& expr, int indent);

        // -------------------- Expression Parsing --------------------
        std::shared_ptr<EXPR> parse_expression();
        std::shared_ptr<EXPR> parse_array_literal();
        std::shared_ptr<EXPR> parse_enum_body();
        std::shared_ptr<EXPR> parse_array_access(std::shared_ptr<EXPR>node);
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
        std::shared_ptr<STMT> parse_enum();
        std::vector<std::shared_ptr<STMT>> parse_block();
        std::shared_ptr<STMT>parse_list();
        std::shared_ptr<STMT>parse_block_stmt();

        // -------------------- Post Parsing ---------------------

        void check_array_rules();
        void check_stmt_array_rules(const std::shared_ptr<STMT>& stmt, bool in_assignment_or_var, const std::string& current_var);
        void check_expr_array_rules(const std::shared_ptr<EXPR>& expr, bool in_assignment_or_var, const std::string& current_var);


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
