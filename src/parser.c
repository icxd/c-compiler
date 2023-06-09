#include <stdlib.h>
#include <stdio.h>
#include "include/sv.h"
#include "include/types.h"
#include "include/parser.h"
#include "include/ast.h"
#include "include/tokenizer.h"


struct parser_t* parser_new(struct tokenizer_t* t) {
    struct parser_t* p = malloc(sizeof(struct parser_t));
    p->tokenizer = t;
    p->ast = NULL;
    p->token = tokenizer_next(t);
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

    while (p->token.type != TK_EOF) {
        if (p->token.type == TK_UNKNOWN) {
            fprintf(stderr, "Unknown token: \""SV_ARG"\" at line %d, column %d\n", SV_FMT(p->token.value), p->token.line, p->token.column);
            break;
        } else if (p->token.type == TK_ERROR) {
            fprintf(stderr, "Error: \""SV_ARG"\" at line %d, column %d\n", SV_FMT(p->token.value), p->token.line, p->token.column);
            break;
        }

        if (p->token.type == TK_COMMENT) {
            p->token = tokenizer_next(p->tokenizer);
            continue;
        }

        struct ast_t* statement = parser_parse_statement(p);
        if (statement == NULL) {
            fprintf(stderr, "Error: Unexpected token: \""SV_ARG"\" (%d) at line %d, column %d\n", SV_FMT(p->token.value), p->token.type, p->token.line, p->token.column);
            break;
        }

        current->next = statement;
        current = current->next;
    }

    return ast;
}

struct ast_t* parser_parse_statement(struct parser_t* p) {
    struct ast_t* statement = malloc(sizeof(struct ast_t));
    statement->next = NULL;

    if (p->token.type == TK_HASH) {
        statement->type = AST_TOP_LEVEL_ATTRIBUTE;
        p->token = tokenizer_next(p->tokenizer);
        string identifier = p->token.value;
        if (sv_compare(identifier, SV("foreign")) == 0) {
            statement->data.top_level_attribute.name = identifier;
            statement->data.top_level_attribute.value = NULL;
            p->token = tokenizer_next(p->tokenizer);

            if (p->token.type == TK_STRING) {
                statement->data.top_level_attribute.value = malloc(sizeof(struct ast_t));
                statement->data.top_level_attribute.value->type = AST_STRING;
                statement->data.top_level_attribute.value->data.string_.value = p->token.value;
                statement->data.top_level_attribute.value->next = NULL;
                p->token = tokenizer_next(p->tokenizer);
            } else {
                fprintf(stderr, "Error: Expected string literal at line %d, column %d\n", p->token.line, p->token.column);
                return NULL;
            }
        } else {
            fprintf(stderr, "Error: Unknown top-level attribute: \""SV_ARG"\" at line %d, column %d\n", SV_FMT(identifier), p->token.line, p->token.column);
            return NULL;
        }
    } else {
        // parse expression statement
        statement->type = AST_EXPRESSION_STATEMENT;
        statement->data.expression_statement.expression = parser_parse_expression(p);

        if (p->token.type != TK_SEMICOLON) {
            fprintf(stderr, "Error: Expected semicolon at line %d, column %d\n", p->token.line, p->token.column);
            return NULL;
        }

        p->token = tokenizer_next(p->tokenizer);
    }
    
    return statement;
}

struct ast_t* parser_parse_expression(struct parser_t* p) {
    return parser_parse_binary(p);
}

struct ast_t* parser_parse_binary(struct parser_t* p) {
    return parser_parse_unary(p);
}

struct ast_t* parser_parse_unary(struct parser_t* p) {
    return parser_parse_primary(p);
}

struct ast_t* parser_parse_primary(struct parser_t* p) {
    struct ast_t* expression = malloc(sizeof(struct ast_t));
    expression->next = NULL;

    struct token_t token = p->token;
    p->token = tokenizer_next(p->tokenizer);

    if (token.type == TK_STRING) {
        expression->type = AST_STRING;
        expression->data.string_.value = token.value;
    } else if (token.type == TK_INTEGER) {
        expression->type = AST_INTEGER;
        expression->data.integer.value = atoi(token.value.chars);
        expression->data.integer.base = BASE_DECIMAL;
    } else if (token.type == TK_FLOAT) {
        expression->type = AST_FLOAT;
        expression->data.float_.value = atof(token.value.chars);
    } else if (token.type == TK_IDENTIFIER) {
        expression->type = AST_VARIABLE;
        expression->data.variable.identifier = token.value;
    } else if (token.type == TK_HEXADECIMAL) {
        expression->type = AST_INTEGER;
        expression->data.integer.value = strtol(token.value.chars, NULL, 16);
        expression->data.integer.base = BASE_HEXADECIMAL;
    } else if (token.type == TK_BINARY) {
        expression->type = AST_INTEGER;
        expression->data.integer.value = strtol(token.value.chars, NULL, 2);
        expression->data.integer.base = BASE_BINARY;
    } else if (token.type == TK_TRUE) {
        expression->type = AST_BOOLEAN;
        expression->data.boolean.value = true;
    } else if (token.type == TK_FALSE) {
        expression->type = AST_BOOLEAN;
        expression->data.boolean.value = false;
    } else if (token.type == TK_NULL) {
        expression->type = AST_NULL;
    } else {
        fprintf(stderr, "Unexpected token: \""SV_ARG"\" (%d) at line %d, column %d\n", SV_FMT(token.value), token.type, token.line, token.column);
        return NULL;
    }
    
    return expression;
}
