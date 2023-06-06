#ifndef AST_H
#define AST_H

#include "sv.h"
#include "types.h"

enum declaration_type_t { };
enum statement_type_t { };
enum expression_type_t { };
enum type_type_t { };

struct declaration_t;
struct statement_t;
struct expression_t;
struct type_t;
struct parameter_t;

struct declaration_t {
    enum declaration_type_t type;
    union {
        struct {
            string name;
            struct type_t* type;
            struct expression_t* value;
        } constant_;
    };
    struct declaration_t* next;
};

struct statement_t {
    enum statement_type_t type;
    union {
        
    };
    struct statement_t* next;
};

struct expression_t {
    enum expression_type_t type;
    union {
        
    };
};

struct type_t {
    enum type_type_t type;
    union {
        
    };
};

#endif // AST_H
