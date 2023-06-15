#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "sv.h"
#include "ast.h"
#include "types.h"

enum symbol_type_t {
    SYMBOL_TYPE_UNKNOWN = 0,
    SYMBOL_TYPE_CONSTANT,
    SYMBOL_TYPE_VARIABLE,
    SYMBOL_TYPE_FUNCTION,
    SYMBOL_TYPE_STRUCT,
    SYMBOL_TYPE_ENUM,
    SYMBOL_TYPE_UNION,
    SYMBOL_TYPE_IMPORT,
};

static const char* symbol_type_names[] = {
    [SYMBOL_TYPE_UNKNOWN] = "unknown",
    [SYMBOL_TYPE_CONSTANT] = "constant",
    [SYMBOL_TYPE_VARIABLE] = "variable",
    [SYMBOL_TYPE_FUNCTION] = "function",
    [SYMBOL_TYPE_STRUCT] = "struct",
    [SYMBOL_TYPE_ENUM] = "enum",
    [SYMBOL_TYPE_UNION] = "union",
    [SYMBOL_TYPE_IMPORT] = "import",
};

struct symbol_t {
    enum symbol_type_t symbol_type;
    string name;
    struct type_t* type;
    struct ast_t* ast;
};

struct symbol_table_t {
    struct symbol_t* symbols;
    uint size;
    uint capacity;
};

struct semantic_analyzer_t {
    struct ast_t* ast;
    struct symbol_table_t* symbol_table;
};

struct semantic_analyzer_t* analyzer_new(struct ast_t* ast);
void analyzer_free(struct semantic_analyzer_t* sa);
void analyzer_lookup_symbols(struct semantic_analyzer_t* sa);
void analyzer_print_symbol_table(struct semantic_analyzer_t* sa);

#endif // SEMANTIC_ANALYZER_H
