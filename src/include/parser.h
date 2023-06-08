#ifndef PARSER_H
#define PARSER_H

#include "tokenizer.h"
#include "ast.h"

struct parser_t {
    struct tokenizer_t* t;
    struct ast_t* ast;
};

struct parser_t* parser_new(struct tokenizer_t* t);
void parser_free(struct parser_t* p);

struct ast_t* parser_parse(struct parser_t* p);
struct ast_t* parser_parse_statement(struct parser_t* p, struct token_t token);
struct ast_t* parser_parse_expression(struct parser_t* p, struct token_t token);

#endif // PARSER_H