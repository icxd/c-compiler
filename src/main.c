#include <stdio.h>
#include <stdlib.h>

#include "include/sv.h"
#include "include/types.h"
#include "include/vm.h"
#include "include/instruction.h"
#include "include/constant.h"

int main(int argc, char** argv) {
    struct vm_t* vm = vm_new();

    vm_write_number(vm, 1);
    vm_write_number(vm, 2);
    vm_write_add(vm);
    vm_write_number(vm, 3);
    vm_write_multiply(vm);
    vm_write_store_global(vm, SV("a"));
    vm_write_load_global(vm, SV("a"));
    vm_write_number(vm, 1);
    vm_write_add(vm);
    vm_write_store_global(vm, SV("b"));
    vm_write_load_global(vm, SV("b"));

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
                    printf(SV_ARG"\n", SV_FMT(instruction.value->string.val));
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
        }
    }

    vm_free(vm);

    return 0;
}