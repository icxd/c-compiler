#include <stdio.h>
#include <stdlib.h>

#include "include/sv.h"
#include "include/types.h"
#include "include/tokenizer.h"
#include "include/file.h"
#include "include/ast.h"
#include "include/parser.h"
#include "include/semantic.h"

// static void print_token(struct token_t token) {
//     printf("type: %d, value: \""SV_ARG"\", line: %d, column: %d\n", token.type, SV_FMT(token.value), token.line, token.column);
// }

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename> [options]\n", argv[0]);
        return 1;
    }

    string filename = SV(argv[1]);
    printf(SV_ARG"\n", SV_FMT(filename));

    struct file_t* file = file_new(filename);
    string contents = file->contents;
    printf(SV_ARG"\n", SV_FMT(contents));

    struct tokenizer_t* tokenizer = tokenizer_new(contents);
    struct parser_t* parser = parser_new(tokenizer);
    struct ast_t* ast = parser_parse(parser);
    struct semantic_analyzer_t* sa = analyzer_new(ast);
    analyzer_lookup_symbols(sa);

    ast_print(ast, 0);
    analyzer_print_symbol_table(sa);

    analyzer_free(sa);
    parser_free(parser);
    tokenizer_free(tokenizer);
    file_free(file);

    return 0;
}
