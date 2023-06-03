#include <stdio.h>
#include <stdlib.h>

#include "include/sv.h"
#include "include/types.h"
#include "include/vm.h"
#include "include/instruction.h"
#include "include/constant.h"

int main(void) {
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
