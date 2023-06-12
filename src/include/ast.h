#ifndef AST_H
#define AST_H

#include "sv.h"
#include "types.h"
#include "tokenizer.h"

enum ast_type_t {
    AST_PROGRAM = 0,

    // Statements
    AST_CONSTANT_DECLARATION,
    AST_TOP_LEVEL_ATTRIBUTE, // TODO: remove this.
    AST_EXPRESSION_STATEMENT,

    // Expressions
    AST_STRING,
    AST_INTEGER,
    AST_FLOAT,
    AST_BOOLEAN,
    AST_NULL,
    AST_BINARY,
    AST_UNARY,
    AST_VARIABLE,
    AST_FUNCTION,
    AST_STRUCT,
    AST_ENUM,
};

enum integer_base_t {
    BASE_DECIMAL = 10,
    BASE_HEXADECIMAL = 16,
    BASE_BINARY = 2,
};

enum type_type_t {
    TYPE_VOID = 0,
    TYPE_CHAR,
    TYPE_I8, TYPE_I16, TYPE_I32, TYPE_I64,
    TYPE_U8, TYPE_U16, TYPE_U32, TYPE_U64,
    TYPE_F32, TYPE_F64,
    TYPE_BOOLEAN,
    TYPE_POINTER,
    TYPE_ARRAY,
    TYPE_GENERIC,
    TYPE_FUNCTION,
    TYPE_UNKNOWN,
};

struct ast_t;

struct type_t {
    enum type_type_t type;
    union {
        struct {
            struct type_t* inner;
        } pointer;
        struct {
            struct type_t* inner;
            bool is_fixed;
            u64 size;
        } array;
        struct {
            string identifier;
            struct type_t* arguments;
            u32 argument_count;
        } generic;
        struct {
            struct type_t* return_type;
            struct type_t* arguments;
            u32 argument_count;
        } function;
        struct {
            string identifier;
        } unknown;
    } data;
};

struct attribute_t {
    string name;
    struct ast_t* value;
};

struct parameter_t {
    string name;
    struct type_t* type;
    struct ast_t* default_value;
    struct parameter_t* next;
};

struct enum_variant_t {
    string name;
    struct ast_t* value;
    struct enum_variant_t* next;
};

union ast_data_t {
    struct {
        string name;
        struct type_t* type;
        struct ast_t* value;
    } const_decl;
    struct {
        string name;
        struct ast_t* value;
    } top_level_attribute;
    struct {
        struct ast_t* expression;
    } expression_statement;

    struct { struct ast_t* statements; } program;
    struct { string value; } string_;
    struct {
        i64 value;
        enum integer_base_t base;
    } integer;
    struct { f64 value; } float_;
    struct { bool value; } boolean;
    struct { int dummy; } null_;
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
    struct {
        struct type_t* return_type; // TODO: multiple return types.
        u32 return_types_count; // unused

        struct parameter_t* parameters;
        
        struct ast_t* body;

        bool foreign;
        string foreign_name;
        bool entry_point;
        bool inline_;

        string variadic_parameter_name;
        bool is_variadic;
    } function;
    struct {
        struct parameter_t* fields; // this uses the parameter_t struct because
                                    // it's basically the same as a function parameter.

        bool foreign;
        string foreign_name;
    } struct_;
    struct {
        struct type_t* base_type;
        struct enum_variant_t* variants;
    } enum_;
};

struct ast_t {
    enum ast_type_t type;
    union ast_data_t data;
    struct ast_t* next;
};

void type_print(struct type_t* type);
void ast_print(struct ast_t* ast, u32 indent);

#endif // AST_H
