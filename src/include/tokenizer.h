#ifndef TOKENIZER
#define TOKENIZER

#include "sv.h"
#include "types.h"

enum token_type_t {
    TOKEN_TYPE_EOF,
    TOKEN_TYPE_IDENTIFIER,
    TOKEN_TYPE_NUMBER,
    TOKEN_TYPE_COLON,
    TOKEN_TYPE_ASSIGN,
    TOKEN_TYPE_ADD,
    TOKEN_TYPE_SEMICOLON,
    TOKEN_TYPE_COMMENT,
    TOKEN_TYPE_UNKNOWN,
    TOKEN_TYPE_ERROR
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

#endif // TOKENIZER
