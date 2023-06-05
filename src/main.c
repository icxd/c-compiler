#include <stdio.h>
#include <stdlib.h>

#include "include/sv.h"
#include "include/types.h"
#include "include/vm.h"
#include "include/instruction.h"
#include "include/constant.h"
#include "include/tokenizer.h"

static void print_token(struct token_t token) {
    printf("type: %d, value: \""SV_ARG"\", line: %d, column: %d\n", token.type, SV_FMT(token.value), token.line, token.column);
}

int main(void) {
    string contents = SV(
        "// In this case, the name for the foreign function can be omitted\n"
        "// since the name of the function is the same as the name of the\n"
        "// foreign function. But I chose to include it for clarity.\n"
        "printf :: fn(fmt: *u8, args: ...any) -> i32 #foreign \"printf\";\n"
        "\n"
        "// The inline function is a function that is expanded at compile time.\n"
        "// This means that the function is not called at runtime, but instead\n"
        "// the function body is inserted at the call site. This is useful for\n"
        "// small functions that are called many times, such as getters and setters.\n"
        "inline_function :: fn() #inline {\n"
            "printf(\"Hello, world!\n\");\n"
        "}\n"
        "\n"
        "// The entry point is the same as the main function in C.\n"
        "// If no function is marked as the entry point, the compiler\n"
        "// will assume that the entry point is called \"main\".\n"
        "main :: fn() -> i32 #entry_point {\n"
            "return 0;\n"
        "}\n"
    );

    struct tokenizer_t* tokenizer = tokenizer_new(contents);
    struct token_t token = tokenizer_next(tokenizer);
    do {
        if (token.type == TK_UNKNOWN) {
            fprintf(stderr, "Unknown token: \""SV_ARG"\" at line %d, column %d\n", SV_FMT(token.value), token.line, token.column);
            break;
        } else if (token.type == TK_ERROR) {
            fprintf(stderr, "Error: \""SV_ARG"\" at line %d, column %d\n", SV_FMT(token.value), token.line, token.column);
            break;
        }
        print_token(token);
        token = tokenizer_next(tokenizer);
    } while (token.type != TK_EOF);

    tokenizer_free(tokenizer);

    ///////////

    struct vm_t* vm = vm_new();

    vm_builtin_function(vm, SV("printf"), 1, true, true, NULL);
    vm_builtin_function(vm, SV("test"), 0, true, false, NULL);

    vm_write_string(vm, SV("Hello, world!"));
    vm_write_call(vm, SV("printf"), 1);
    
    vm_write_string(vm, SV("%d"));
    vm_write_number(vm, 1);
    vm_write_number(vm, 2);
    vm_write_add(vm);
    vm_write_call(vm, SV("printf"), 2);

    vm_write_call(vm, SV("test"), 0);

    printf("size: %d\n", vm->chunk->size);
    for (uint i = 0; i < vm->chunk->size; i++) {
        struct instruction_t instruction = vm->chunk->instructions[i];
        switch (instruction.op) {
            case OP_CONSTANT: {
                printf("%d: OP_CONSTANT ", i);
                if (instruction.value_type == CONSTANT_VALUE_TYPE_IDX) {
                    printf("%d\n", instruction.value->idx.index);
                } else if (instruction.value_type == CONSTANT_VALUE_TYPE_NUMBER) {
                    printf("%d\n", instruction.value->number.val);
                } else {
                    printf("\""SV_ARG"\"\n", SV_FMT(instruction.value->string.val));
                }
                break;
            }
            case OP_LOAD_GLOBAL: {
                printf("%d: OP_LOAD_GLOBAL %d ("SV_ARG")\n", i, instruction.value->idx.index, SV_FMT(vm->globals->symbols[instruction.value->idx.index].name));
                break;
            }
            case OP_STORE_GLOBAL: {
                printf("%d: OP_STORE_GLOBAL %d ("SV_ARG")\n", i, instruction.value->idx.index, SV_FMT(vm->globals->symbols[instruction.value->idx.index].name));
                break;
            }
            case OP_POP: {
                printf("%d: OP_POP\n", i);
                break;
            }
            case OP_ADD: {
                printf("%d: OP_ADD\n", i);
                break;
            }
            case OP_SUBTRACT: {
                printf("%d: OP_SUBTRACT\n", i);
                break;
            }
            case OP_MULTIPLY: {
                printf("%d: OP_MULTIPLY\n", i);
                break;
            }
            case OP_DIVIDE: {
                printf("%d: OP_DIVIDE\n", i);
                break;
            }
            case OP_NEGATE: {
                printf("%d: OP_NEGATE\n", i);
                break;
            }
            case OP_RETURN: {
                printf("%d: OP_RETURN\n", i);
                break;
            }
            case OP_CALL: {
                printf("%d: OP_CALL %d ("SV_ARG") (%d)\n", i,
                    instruction.call.name_index, 
                    SV_FMT(vm->globals->symbols[instruction.call.name_index].name),
                    instruction.call.arity);
                break;
            }
        }
    }

    vm_free(vm);

    return 0;
}
