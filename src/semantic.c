#include <stdio.h>
#include <stdlib.h>
#include "include/semantic.h"

struct semantic_analyzer_t* analyzer_new(struct ast_t* ast) {
    struct semantic_analyzer_t* sa = malloc(sizeof(struct semantic_analyzer_t));
    sa->ast = ast;
    sa->symbol_table = malloc(sizeof(struct symbol_table_t));
    sa->symbol_table->size = 0;
    sa->symbol_table->capacity = 0;
    sa->symbol_table->symbols = NULL;
    return sa;
}

void analyzer_free(struct semantic_analyzer_t* sa) {
    free(sa->symbol_table->symbols);
    free(sa->symbol_table);
    free(sa);
}

static void analyzer_add_symbol(struct semantic_analyzer_t* sa, struct symbol_t* symbol) {
    if (sa->symbol_table->size == sa->symbol_table->capacity) {
        sa->symbol_table->capacity = sa->symbol_table->capacity == 0 ? 1 : sa->symbol_table->capacity * 2;
        sa->symbol_table->symbols = realloc(sa->symbol_table->symbols, sizeof(struct symbol_t) * sa->symbol_table->capacity);
    }

    sa->symbol_table->symbols[sa->symbol_table->size++] = *symbol;
}

static struct symbol_t* analyzer_find_symbol(struct semantic_analyzer_t* sa, string name) {
    for (unsigned int i = 0; i < sa->symbol_table->size; i++) {
        if (sv_compare(sa->symbol_table->symbols[i].name, name)) {
            return &sa->symbol_table->symbols[i];
        }
    }

    return NULL;
}

void analyzer_lookup_symbols(struct semantic_analyzer_t* sa) {
    struct ast_t* ast = sa->ast;
    
    do {
        switch (ast->type) {
            case AST_CONSTANT_DECLARATION: {
                if (analyzer_find_symbol(sa, ast->data.const_decl.name) != NULL) {
                    fprintf(stderr, "error: redeclaration of symbol \""SV_ARG"\"\n", SV_FMT(ast->data.const_decl.name));
                    exit(1);
                }

                struct symbol_t* symbol = malloc(sizeof(struct symbol_t));

                if (ast->data.const_decl.value != NULL) {
                    struct ast_t* value = ast->data.const_decl.value;
                    
                    switch (value->type) {
                        case AST_ENUM: symbol->symbol_type = SYMBOL_TYPE_ENUM; break;
                        case AST_STRUCT: symbol->symbol_type = SYMBOL_TYPE_STRUCT; break;
                        case AST_UNION: symbol->symbol_type = SYMBOL_TYPE_UNION; break;
                        case AST_FUNCTION: symbol->symbol_type = SYMBOL_TYPE_FUNCTION; break;
                        case AST_IMPORT: symbol->symbol_type = SYMBOL_TYPE_IMPORT; break;
                        default: symbol->symbol_type = SYMBOL_TYPE_CONSTANT; break;
                    }
                } else {
                    symbol->symbol_type = SYMBOL_TYPE_CONSTANT;
                }

                symbol->name = ast->data.const_decl.name;
                symbol->type = ast->data.const_decl.type;
                symbol->ast = ast;

                analyzer_add_symbol(sa, symbol);
            }
            default: {
                ast = ast->next;
                continue;
            }
        }

        ast = ast->next;
    } while (ast != NULL);
}

void analyzer_print_symbol_table(struct semantic_analyzer_t* sa) {
    printf("Symbol table:\n");
    for (uint i = 0; i < sa->symbol_table->size; i++) {
        struct symbol_t symbol = sa->symbol_table->symbols[i];
        printf("name: "SV_ARG", type: ", SV_FMT(symbol.name));
        if (symbol.type != NULL) type_print(symbol.type);
        else printf("<null>");
        printf(", symbol type: %s (%d)\n", symbol_type_names[symbol.symbol_type], symbol.symbol_type);
    }
}
