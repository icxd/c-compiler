#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "sv.h"
#include "types.h"

enum token_type_t {
    TK_EOF,

    TK_IDENTIFIER,
    TK_STRING,
    TK_INTEGER,
    TK_FLOAT,
    TK_HEXADECIMAL,
    TK_BINARY,

    TK_FN,
    TK_STRUCT,
    TK_ENUM,
    TK_UNION,
    TK_USING,
    TK_IMPORT,
    TK_TRUE, TK_FALSE, TK_NULL,
    TK_VOID,
    TK_CHAR,
    TK_I8, TK_I16, TK_I32, TK_I64,
    TK_U8, TK_U16, TK_U32, TK_U64,
    TK_F32, TK_F64,
    TK_BOOL,

    TK_OPEN_PAREN,
    TK_CLOSE_PAREN,
    TK_OPEN_BRACE,
    TK_CLOSE_BRACE,
    TK_OPEN_BRACKET,
    TK_CLOSE_BRACKET,
    TK_PERIOD,
    TK_COMMA,
    TK_COLON,
    TK_SEMICOLON,
    TK_ARROW,
    TK_HASH,

    TK_ASSIGN,
    TK_PLUS_ASSIGN,
    TK_PLUS,
    TK_MINUS_ASSIGN,
    TK_MINUS,
    TK_ASTERISK_ASSIGN,
    TK_ASTERISK,
    TK_SLASH_ASSIGN,
    TK_SLASH,
    TK_PERCENT_ASSIGN,
    TK_PERCENT,

    TK_COMMENT,
    TK_UNKNOWN,
    TK_ERROR
};

struct tokenizer_t {
    string contents;
    uint index;
    uint line;
    uint column;
};

struct token_t {
    enum token_type_t type;
    string value;
    uint line;
    uint column;
};

struct tokenizer_t* tokenizer_new(string contents);
void tokenizer_free(struct tokenizer_t* tokenizer);
struct token_t tokenizer_next(struct tokenizer_t* tokenizer);
char tokenizer_peek(struct tokenizer_t* tokenizer, uint offset);

#endif // TOKENIZER_H
