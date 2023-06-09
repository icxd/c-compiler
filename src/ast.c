#include <stdio.h>
#include <stdlib.h>
#include "include/ast.h"
#include "include/types.h"

static char* bianry_repr(int n) {
    int len = sizeof(int) * 8;
    char* binary = (char*)malloc(sizeof(char) * len);
    int k = 0;
    for (unsigned i = (1 << (len - 1)); i > 0; i = i / 2)
        binary[k++] = (n & i) ? '1' : '0';
    binary[k] = '\0';
    int i = 0, j = 0;
    while (binary[i] == '0') i++;
    while (binary[i]) binary[j++] = binary[i++];
    binary[j] = '\0';
    return binary;
}

void type_print(struct type_t* type) {
    if (type == NULL) {
        return;
    }

    switch (type->type) {
        case TYPE_VOID: printf("void"); break;
        case TYPE_CHAR: printf("char"); break;
        case TYPE_I8: printf("i8"); break;
        case TYPE_I16: printf("i16"); break;
        case TYPE_I32: printf("i32"); break;
        case TYPE_I64: printf("i64"); break;
        case TYPE_U8: printf("u8"); break;
        case TYPE_U16: printf("u16"); break;
        case TYPE_U32: printf("u32"); break;
        case TYPE_U64: printf("u64"); break;
        case TYPE_F32: printf("f32"); break;
        case TYPE_F64: printf("f64"); break;
        case TYPE_BOOLEAN: printf("boolean"); break;
        case TYPE_POINTER: {
            printf("*");
            type_print(type->data.pointer.inner);
            break;
        }
        case TYPE_ARRAY: {
            printf("[");
            if (type->data.array.is_fixed) {
                printf("%d", type->data.array.size);
            }
            printf("]");
            type_print(type->data.array.inner);
            break;
        }
        case TYPE_UNKNOWN: {
            printf("unknown ("SV_ARG")", SV_FMT(type->data.unknown.identifier));
            break;
        }
        default: printf("<unknown type>"); break;
    }
}

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

        case AST_CONSTANT_DECLARATION: {
            printf("AST_CONSTANT_DECLARATION: \""SV_ARG"\"", SV_FMT(ast->data.const_decl.name));
            if (ast->data.const_decl.type != NULL) {
                printf(" : ");
                type_print(ast->data.const_decl.type);
            } else {
                printf(" : <null>");
            }
            printf("\n");
            ast_print(ast->data.const_decl.value, indent + 1);
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
            printf("AST_INTEGER: ");
            switch (ast->data.integer.base) {
                case BASE_DECIMAL: {
                    printf("%lld\n", ast->data.integer.value);
                    break;
                }
                case BASE_HEXADECIMAL: {
                    printf("0x%llx (%lld)\n", ast->data.integer.value, ast->data.integer.value);
                    break;
                }
                case BASE_BINARY: {
                    printf("0b%s (%lld)\n", bianry_repr(ast->data.integer.value), ast->data.integer.value);
                    break;
                }
                default: {
                    printf("Unknown integer base: %d\n", ast->data.integer.base);
                    break;
                }
            }
            break;
        }
        case AST_FLOAT: {
            printf("AST_FLOAT: %f\n", ast->data.float_.value);
            break;
        }
        case AST_BOOLEAN: {
            printf("AST_BOOLEAN: %s\n", ast->data.boolean.value ? "true" : "false");
            break;
        }
        case AST_NULL: {
            printf("AST_NULL\n");
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
        case AST_FUNCTION: {
            printf("AST_FUNCTION : ");
            if (ast->data.function.return_type != NULL) {
                type_print(ast->data.function.return_type);
            } else {
                printf("<null>");
            }
            if (ast->data.function.foreign) {
                printf(" #foreign "SV_ARG, SV_FMT(ast->data.function.foreign_name));
            }
            printf("\n");
            struct parameter_t* param = ast->data.function.parameters;
            while (param != NULL) {
                for (u32 i = 0; i < indent + 1; i++) {
                    printf("    ");
                }
                printf("Parameter: \""SV_ARG"\"", SV_FMT(param->name));
                if (param->type != NULL) {
                    printf(" : ");
                    type_print(param->type);
                } else {
                    printf(" : <null>");
                }
                printf("\n");
                param = param->next;
            }
            ast_print(ast->data.function.body, indent + 1);
            break;
        }
        default: {
            printf("Unknown AST type: %d\n", ast->type);
            break;
        }
    }

    ast_print(ast->next, indent);
}
