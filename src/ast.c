#include <stdio.h>
#include "include/ast.h"
#include "include/types.h"

void ast_print(struct ast_t* ast, u32 indent) {
    if (ast == NULL) {
        return;
    }

    for (u32 i = 0; i < indent; i++) {
        printf("    ");
    }

    switch (ast->type) {
        case AST_PROGRAM: {
            printf("AST_PROGRAM\n");
            ast_print(ast->data.program.statements, indent + 1);
            break;
        }
        case AST_STRING: {
            printf("AST_STRING: \""SV_ARG"\"\n", SV_FMT(ast->data.string_.value));
            break;
        }
        case AST_INTEGER: {
            printf("AST_INTEGER: %lld\n", ast->data.integer.value);
            break;
        }
        case AST_FLOAT: {
            printf("AST_FLOAT: %f\n", ast->data.float_.value);
            break;
        }
        case AST_BINARY: {
            break;
        }
        case AST_UNARY: {
            break;
        }
        case AST_VARIABLE: {
            printf("AST_VARIABLE: \""SV_ARG"\"\n", SV_FMT(ast->data.variable.identifier));
            break;
        }
        default: {
            printf("Unknown AST type: %d\n", ast->type);
            break;
        }
    }

    ast_print(ast->next, indent);
}
