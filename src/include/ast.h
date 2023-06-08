#ifndef AST_H
#define AST_H

#include "sv.h"
#include "types.h"
#include "tokenizer.h"

enum ast_type_t {
    AST_PROGRAM = 0,

    // Statements
    AST_TOP_LEVEL_ATTRIBUTE, // TODO: remove this.
    AST_EXPRESSION_STATEMENT,

    // Expressions
    AST_STRING,
    AST_INTEGER,
    AST_FLOAT,
    AST_BINARY,
    AST_UNARY,
    AST_VARIABLE,
};

struct ast_t;

union ast_data_t {
    struct {
        string name;
        struct ast_t* value;
    } top_level_attribute;
    struct {
        struct ast_t* expression;
    } expression_statement;

    struct { struct ast_t* statements; } program;
    struct { string value; } string_;
    struct { i64 value; } integer;
    struct { f64 value; } float_;
    struct {
        struct token_t op;
        struct ast_t* left;
        struct ast_t* right;
    } binary_op;
    struct {
        struct token_t op;
        struct ast_t* left;
    } unary_op;
    struct { string identifier; } variable;
};

struct ast_t {
    enum ast_type_t type;
    union ast_data_t data;
    struct ast_t* next;
};

void ast_print(struct ast_t* ast, u32 indent);

#endif // AST_H
