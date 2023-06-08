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

        struct ast_t* statement = parser_parse_statement(p, &token);
        if (statement == NULL) {
            fprintf(stderr, "Error: Unexpected token: \""SV_ARG"\" (%d) at line %d, column %d\n", SV_FMT(token.value), token.type, token.line, token.column);
            break;
        }

        current->next = statement;
        current = current->next;
        // token = tokenizer_next(p->t);
    }

    return ast;
}

struct ast_t* parser_parse_statement(struct parser_t* p, struct token_t* token) {
    struct ast_t* statement = malloc(sizeof(struct ast_t));
    statement->next = NULL;

    if ((*token).type == TK_HASH) {
        statement->type = AST_TOP_LEVEL_ATTRIBUTE;
        *token = tokenizer_next(p->t);
        string identifier = (*token).value;
        if (sv_compare(identifier, SV("foreign")) == 0) {
            statement->data.top_level_attribute.name = identifier;
            statement->data.top_level_attribute.value = NULL;
            *token = tokenizer_next(p->t);

            if ((*token).type == TK_STRING) {
                statement->data.top_level_attribute.value = malloc(sizeof(struct ast_t));
                statement->data.top_level_attribute.value->type = AST_STRING;
                statement->data.top_level_attribute.value->data.string_.value = (*token).value;
                statement->data.top_level_attribute.value->next = NULL;
                *token = tokenizer_next(p->t);
            } else {
                fprintf(stderr, "Error: Expected string literal at line %d, column %d\n", (*token).line, (*token).column);
                return NULL;
            }
        } else {
            fprintf(stderr, "Error: Unknown top-level attribute: \""SV_ARG"\" at line %d, column %d\n", SV_FMT(identifier), (*token).line, (*token).column);
            return NULL;
        }
    } else {
        return parser_parse_expression(p, token);
    }
    
    return statement;
}

struct ast_t* parser_parse_expression(struct parser_t* p, struct token_t* token) {
    return parser_parse_binary(p, token);
}

struct ast_t* parser_parse_binary(struct parser_t* p, struct token_t* token) {
    struct ast_t* expression = parser_parse_unary(p, token);
    *token = tokenizer_next(p->t);

    while ((*token).type == TK_PLUS || 
           (*token).type == TK_MINUS ||
           (*token).type == TK_ASTERISK ||
           (*token).type == TK_SLASH ||
           (*token).type == TK_PERCENT) {
        struct ast_t* new_expression = malloc(sizeof(struct ast_t));
        new_expression->type = AST_BINARY;
        new_expression->data.binary_op.op = *token;
        new_expression->data.binary_op.left = expression;
        struct token_t right = tokenizer_next(p->t);
        new_expression->data.binary_op.right = parser_parse_unary(p, &right);
        new_expression->next = NULL;

        expression = new_expression;
        *token = tokenizer_next(p->t);
    }

    return expression;
}

struct ast_t* parser_parse_unary(struct parser_t* p, struct token_t* token) {
    return parser_parse_primary(p, token);
}

struct ast_t* parser_parse_primary(struct parser_t* p, struct token_t* token) {
    (void)p; // unused
    struct ast_t* expression = malloc(sizeof(struct ast_t));
    expression->next = NULL;

    if ((*token).type == TK_STRING) {
        expression->type = AST_STRING;
        expression->data.string_.value = (*token).value;
    } else if ((*token).type == TK_INTEGER) {
        expression->type = AST_INTEGER;
        expression->data.integer.value = atoi((*token).value.chars);
    } else if ((*token).type == TK_FLOAT) {
        expression->type = AST_FLOAT;
        expression->data.float_.value = atof((*token).value.chars);
    } else if ((*token).type == TK_IDENTIFIER) {
        expression->type = AST_VARIABLE;
        expression->data.variable.identifier = (*token).value;
    } else if ((*token).type == TK_HEXADECIMAL) {
        expression->type = AST_INTEGER;
        expression->data.integer.value = strtol((*token).value.chars, NULL, 16);
    } else if ((*token).type == TK_BINARY) {
        expression->type = AST_INTEGER;
        expression->data.integer.value = strtol((*token).value.chars, NULL, 2);
    } else {
        fprintf(stderr, "Unexpected token: \""SV_ARG"\" (%d) at line %d, column %d\n", SV_FMT((*token).value), (*token).type, (*token).line, (*token).column);
        return NULL;
    }

    *token = tokenizer_next(p->t);
    
    return expression;
}
