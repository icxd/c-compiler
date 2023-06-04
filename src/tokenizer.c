#include <stdlib.h>
#include "include/tokenizer.h"
#include "include/sv.h"

static b32 is_alpha(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }
static b32 is_digit(char c) { return c >= '0' && c <= '9'; }
static b32 is_alphanumeric(char c) { return is_alpha(c) || is_digit(c); }

struct tokenizer_t* tokenizer_new(string contents) {
    struct tokenizer_t* tokenizer = malloc(sizeof(struct tokenizer_t));
    tokenizer->contents = contents;
    tokenizer->index = 0;
    tokenizer->line = 1;
    tokenizer->column = 1;
    return tokenizer;
}

void tokenizer_free(struct tokenizer_t* tokenizer) {
    free(tokenizer);
}

struct token_t tokenizer_next(struct tokenizer_t* tokenizer) {
    struct token_t token = {
        .type = TOKEN_TYPE_EOF,
        .line = tokenizer->line,
        .column = tokenizer->column
    };

    if (tokenizer->index >= tokenizer->contents.size) {
        return token;
    }

    char c = tokenizer->contents.chars[tokenizer->index];
    if (c == '\n') {
        tokenizer->line++;
        tokenizer->column = 1;
        tokenizer->index++;
        return tokenizer_next(tokenizer);
    } else if (c == ':') {
        token.type = TOKEN_TYPE_COLON;
        tokenizer->column++;
        tokenizer->index++;
        return token;
    } else if (c == '+') {
        token.type = TOKEN_TYPE_ADD;
        tokenizer->column++;
        tokenizer->index++;
        return token;
    } else if (c == '=') {
        token.type = TOKEN_TYPE_ASSIGN;
        tokenizer->column++;
        tokenizer->index++;
        return token;
    } else if (c == ';') {
        token.type = TOKEN_TYPE_SEMICOLON;
        tokenizer->column++;
        tokenizer->index++;
        return token;
    } else if (c == '/') {
        if (tokenizer->contents.chars[tokenizer->index + 1] == '/') {
            token.type = TOKEN_TYPE_COMMENT;
            tokenizer->column += 2;
            tokenizer->index += 2;
            return token;
        } else {
            token.type = TOKEN_TYPE_UNKNOWN;
            tokenizer->column++;
            tokenizer->index++;
            return token;
        }
        return token;
    } else if (c == ' ' || c == '\t') {
        tokenizer->column++;
        tokenizer->index++;
        return tokenizer_next(tokenizer);
    } else {
        if (is_digit(c)) {
            token.type = TOKEN_TYPE_NUMBER;
            unsigned int start = tokenizer->index;
            while (is_digit(tokenizer->contents.chars[tokenizer->index])) {
                tokenizer->column++;
                tokenizer->index++;
            }
            token.value = sv_substr(tokenizer->contents, start, tokenizer->index);
            return token;
        } else if (is_alpha(c)) {
            token.type = TOKEN_TYPE_IDENTIFIER;
            unsigned int start = tokenizer->index;
            while (is_alphanumeric(tokenizer->contents.chars[tokenizer->index])) {
                tokenizer->column++;
                tokenizer->index++;
            }
            token.value = sv_substr(tokenizer->contents, start, tokenizer->index);
            return token;
        } else {
            token.type = TOKEN_TYPE_UNKNOWN;
            tokenizer->column++;
            tokenizer->index++;
            return token;
        }
    }

    return token;
}

