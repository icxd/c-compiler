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

        case AST_TOP_LEVEL_ATTRIBUTE: {
            printf("AST_TOP_LEVEL_ATTRIBUTE: \""SV_ARG"\"\n", SV_FMT(ast->data.top_level_attribute.name));
            if (ast->data.top_level_attribute.value != NULL) {
                ast_print(ast->data.top_level_attribute.value, indent + 1);
            }
            break;
        }
        case AST_EXPRESSION_STATEMENT: {
            printf("AST_EXPRESSION_STATEMENT\n");
            ast_print(ast->data.expression_statement.expression, indent + 1);
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
            printf("AST_BINARY: "SV_ARG"\n", SV_FMT(ast->data.binary_op.op.value));
            ast_print(ast->data.binary_op.left, indent + 1);
            ast_print(ast->data.binary_op.right, indent + 1);
            break;
        }
        case AST_UNARY: {
            printf("AST_UNARY: "SV_ARG"\n", SV_FMT(ast->data.unary_op.op.value));
            ast_print(ast->data.unary_op.left, indent + 1);
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
