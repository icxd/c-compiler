#include <stdlib.h>
#include <stdio.h>
#include "include/sv.h"
#include "include/types.h"
#include "include/parser.h"
#include "include/ast.h"
#include "include/tokenizer.h"


struct parser_t* parser_new(struct tokenizer_t* t) {
    struct parser_t* p = malloc(sizeof(struct parser_t));
    p->t = t;
    p->ast = NULL; // temporary
    return p;
}

void parser_free(struct parser_t* p) {
    free(p);
}

struct ast_t* parser_parse(struct parser_t* p) {
    struct ast_t* ast = malloc(sizeof(struct ast_t));
    ast->type = AST_PROGRAM;
    ast->data.program.statements = NULL;
    ast->next = NULL;

    struct ast_t* current = ast;

    struct token_t token = tokenizer_next(p->t);
    while (token.type != TK_EOF) {
        if (token.type == TK_UNKNOWN) {
            fprintf(stderr, "Unknown token: \""SV_ARG"\" at line %d, column %d\n", SV_FMT(token.value), token.line, token.column);
            break;
        } else if (token.type == TK_ERROR) {
            fprintf(stderr, "Error: \""SV_ARG"\" at line %d, column %d\n", SV_FMT(token.value), token.line, token.column);
            break;
        }

        if (token.type == TK_COMMENT) {
            token = tokenizer_next(p->t);
            continue;
        }

        current->next = parser_parse_statement(p, token);
        current = current->next;
        token = tokenizer_next(p->t);
    }

    return ast;
}

struct ast_t* parser_parse_statement(struct parser_t* p, struct token_t token) {
    return parser_parse_expression(p, token);
}

struct ast_t* parser_parse_expression(struct parser_t* p, struct token_t token) {
    (void)p; // unused
    struct ast_t* expression = malloc(sizeof(struct ast_t));
    expression->next = NULL;

    if (token.type == TK_STRING) {
        expression->type = AST_STRING;
        expression->data.string_.value = token.value;
    } else if (token.type == TK_INTEGER) {
        expression->type = AST_INTEGER;
        expression->data.integer.value = atoi(token.value.chars);
    } else if (token.type == TK_FLOAT) {
        expression->type = AST_FLOAT;
        expression->data.float_.value = atof(token.value.chars);
    } else if (token.type == TK_IDENTIFIER) {
        expression->type = AST_VARIABLE;
        expression->data.variable.identifier = token.value;
    } else if (token.type == TK_HEXADECIMAL) {
        expression->type = AST_INTEGER;
        expression->data.integer.value = strtol(token.value.chars, NULL, 16);
    } else if (token.type == TK_BINARY) {
        expression->type = AST_INTEGER;
        expression->data.integer.value = strtol(token.value.chars, NULL, 2);
    } else {
        fprintf(stderr, "Unexpected token: \""SV_ARG"\" at line %d, column %d\n", SV_FMT(token.value), token.line, token.column);
        return NULL;
    }
    
    return expression;
}
