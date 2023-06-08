#include <stdlib.h>
#include "include/tokenizer.h"
#include "include/sv.h"

static b32 is_alpha(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }
static b32 is_digit(char c) { return c >= '0' && c <= '9'; }
static b32 is_alphanumeric(char c) { return is_alpha(c) || is_digit(c); }
static b32 is_hex(char c) { return is_digit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'); }

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
        .type = TK_EOF,
        .line = tokenizer->line,
        .column = tokenizer->column
    };

    if (tokenizer->index >= tokenizer->contents.size) {
        return token;
    }

    #define SINGLE_TOKEN(chr, ty) \
        if (c == chr) { \
            token.type = ty; \
            token.value = sv_substr(tokenizer->contents, tokenizer->index, tokenizer->index + 1); \
            tokenizer->column++; \
            tokenizer->index++; \
            return token; \
        }
    #define DOUBLE_TOKEN(first, second, ty) \
        if (c == first && tokenizer_peek(tokenizer, 1) == second) { \
            token.type = ty; \
            token.value = sv_substr(tokenizer->contents, tokenizer->index, tokenizer->index + 2); \
            tokenizer->column += 2; \
            tokenizer->index += 2; \
            return token; \
        }

    char c = sv_at(tokenizer->contents, tokenizer->index);
    if (c == '\n') {
        tokenizer->line++;
        tokenizer->column = 1;
        tokenizer->index++;
        return tokenizer_next(tokenizer);
    }
    SINGLE_TOKEN('(', TK_OPEN_PAREN);
    SINGLE_TOKEN(')', TK_CLOSE_PAREN);
    SINGLE_TOKEN('{', TK_OPEN_BRACE);
    SINGLE_TOKEN('}', TK_CLOSE_BRACE);
    SINGLE_TOKEN('[', TK_OPEN_BRACKET);
    SINGLE_TOKEN(']', TK_CLOSE_BRACKET);
    SINGLE_TOKEN('.', TK_PERIOD);
    SINGLE_TOKEN(',', TK_COMMA);
    SINGLE_TOKEN(':', TK_COLON);
    SINGLE_TOKEN(';', TK_SEMICOLON);
    DOUBLE_TOKEN('-', '>', TK_ARROW);
    SINGLE_TOKEN('#', TK_HASH);
    SINGLE_TOKEN('=', TK_ASSIGN);
    DOUBLE_TOKEN('+', '=', TK_PLUS_ASSIGN);
    SINGLE_TOKEN('+', TK_PLUS);
    DOUBLE_TOKEN('-', '=', TK_MINUS_ASSIGN);
    SINGLE_TOKEN('-', TK_MINUS);
    DOUBLE_TOKEN('*', '=', TK_ASTERISK_ASSIGN);
    SINGLE_TOKEN('*', TK_ASTERISK);
    DOUBLE_TOKEN('%', '=', TK_PERCENT_ASSIGN);
    SINGLE_TOKEN('%', TK_PERCENT);
    if (c == '/' && sv_at(tokenizer->contents, tokenizer->index + 1) == '/') {
        token.type = TK_COMMENT;
        unsigned int start = tokenizer->index;
        while (sv_at(tokenizer->contents, tokenizer->index) != '\n') {
            tokenizer->column++;
            tokenizer->index++;
        }
        token.value = sv_substr(tokenizer->contents, start, tokenizer->index);
        return token;
    }
    DOUBLE_TOKEN('/', '=', TK_SLASH_ASSIGN);
    SINGLE_TOKEN('/', TK_SLASH);
    if (c == ' ' || c == '\t') {
        tokenizer->column++;
        tokenizer->index++;
        return tokenizer_next(tokenizer);
    }
    if (c == '\r') {
        tokenizer->column++;
        tokenizer->index++;
        return tokenizer_next(tokenizer);
    }
    if (c == '"') {
        tokenizer->column++;
        tokenizer->index++;
        token.type = TK_STRING;
        unsigned int start = tokenizer->index;
        while (sv_at(tokenizer->contents, tokenizer->index) != '"') {
            tokenizer->column++;
            tokenizer->index++;
        }
        tokenizer->column++;
        tokenizer->index++;
        token.value = sv_substr(tokenizer->contents, start, tokenizer->index - 1);
        return token;
    }
    if (is_digit(c)) {
        if (c == '0') {
            if (tokenizer_peek(tokenizer, 1) == 'x') {
                tokenizer->column += 2;
                tokenizer->index += 2;
                token.type = TK_HEXADECIMAL;
                unsigned int start = tokenizer->index;
                while (is_hex(sv_at(tokenizer->contents, tokenizer->index))) {
                    tokenizer->column++;
                    tokenizer->index++;
                }
                token.value = sv_substr(tokenizer->contents, start, tokenizer->index);
                return token;
            } else if (tokenizer_peek(tokenizer, 1) == 'b') {
                tokenizer->column += 2;
                tokenizer->index += 2;
                token.type = TK_BINARY;
                unsigned int start = tokenizer->index;
                while (sv_at(tokenizer->contents, tokenizer->index) == '0' || sv_at(tokenizer->contents, tokenizer->index) == '1') {
                    tokenizer->column++;
                    tokenizer->index++;
                }
                token.value = sv_substr(tokenizer->contents, start, tokenizer->index);
                return token;
            }
        }
        token.type = TK_INTEGER;
        unsigned int start = tokenizer->index;
        while (is_digit(sv_at(tokenizer->contents, tokenizer->index))) {
            tokenizer->column++;
            tokenizer->index++;
        }
        if (sv_at(tokenizer->contents, tokenizer->index) == '.') {
            tokenizer->column++;
            tokenizer->index++;
            while (is_digit(sv_at(tokenizer->contents, tokenizer->index))) {
                tokenizer->column++;
                tokenizer->index++;
            }
            token.type = TK_FLOAT;
        }
        token.value = sv_substr(tokenizer->contents, start, tokenizer->index);
        return token;
    }
    if (is_alpha(c) || c == '_') {
        token.type = TK_IDENTIFIER;
        unsigned int start = tokenizer->index;
        while (is_alphanumeric(sv_at(tokenizer->contents, tokenizer->index)) || sv_at(tokenizer->contents, tokenizer->index) == '_') {
            tokenizer->column++;
            tokenizer->index++;
        }
        token.value = sv_substr(tokenizer->contents, start, tokenizer->index);
        return token;
    }
    
    token.type = TK_UNKNOWN;
    token.value = sv_substr(tokenizer->contents, tokenizer->index, tokenizer->index + 1);
    tokenizer->column++;
    tokenizer->index++;
    return token;

    #undef SINGLE_TOKEN
    #undef DOUBLE_TOKEN

    return token;
}

char tokenizer_peek(struct tokenizer_t* tokenizer, uint offset) {
    return sv_at(tokenizer->contents, tokenizer->index + offset);
}
