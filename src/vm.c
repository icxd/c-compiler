#include <stdio.h>
#include <stdlib.h>

#include "include/vm.h"
#include "include/sv.h"

struct vm_t* vm_new(void) {
    struct vm_t* vm = malloc(sizeof(struct vm_t));
    vm->chunk = malloc(sizeof(struct chunk_t));
    vm->chunk->instructions = malloc(sizeof(struct instruction_t) * 256);
    vm->chunk->size = 0;
    vm->chunk->capacity = 256;
    vm->ip = 0;
    vm->globals = malloc(sizeof(struct symbol_table_t));
    vm->globals->symbols = malloc(sizeof(struct symbol_t) * 256);
    vm->globals->size = 0;
    vm->globals->capacity = 256;
    vm->stack_size = 0;
    vm->builtins = malloc(sizeof(struct function_t) * 256);
    vm->builtin_func_count = 0;
    return vm;
}

void vm_free(struct vm_t* vm) {
    free(vm->chunk->instructions);
    free(vm->chunk);
    free(vm->globals->symbols);
    free(vm->globals);
    free(vm);
}

void vm_write(struct vm_t* vm, struct instruction_t instruction) {
    if (vm->chunk->size >= vm->chunk->capacity) {
        vm->chunk->capacity *= 2;
        vm->chunk->instructions = realloc(vm->chunk->instructions, sizeof(struct instruction_t) * vm->chunk->capacity);
    }
    vm->chunk->instructions[vm->chunk->size++] = instruction;
}

void vm_write_constant(struct vm_t* vm, union constant_value_t value) {
    struct instruction_t instruction = {
        .op = OP_CONSTANT,
        .value_type = CONSTANT_VALUE_TYPE_NUMBER,
        .value = malloc(sizeof(union constant_value_t))
    };
    *instruction.value = value;
    vm_write(vm, instruction);
}

void vm_write_store_global(struct vm_t* vm, string name) {
    struct instruction_t instruction = {
        .op = OP_STORE_GLOBAL,
        .value = malloc(sizeof(union constant_value_t))
    };
    u32 index = vm->globals->size;
    vm->globals->symbols[index].name = name;
    vm->globals->symbols[index].index = index;
    vm->globals->size++;
    instruction.value->idx.index = index;
    vm_write(vm, instruction);
}

void vm_write_load_global(struct vm_t* vm, string name) {
    struct instruction_t instruction = {
        .op = OP_LOAD_GLOBAL,
        .value = malloc(sizeof(union constant_value_t))
    };
    u32 index = 0;
    for (uint idx = 0; idx < vm->globals->size; idx++) {
        if (vm->globals->symbols[idx].name.size == name.size && strncmp(vm->globals->symbols[idx].name.chars, name.chars, name.size) == 0) {
            index = vm->globals->symbols[idx].index;
            break;
        }
    }
    instruction.value->idx.index = index;
    vm_write(vm, instruction);
}

void vm_write_add(struct vm_t* vm) {
    struct instruction_t instruction = {
        .op = OP_ADD,
        .value_type = CONSTANT_VALUE_TYPE_NONE,
    };
    vm_write(vm, instruction);
}

void vm_write_subtract(struct vm_t* vm) {
    struct instruction_t instruction = {
        .op = OP_SUBTRACT,
        .value_type = CONSTANT_VALUE_TYPE_NONE,
    };
    vm_write(vm, instruction);
}

void vm_write_multiply(struct vm_t* vm) {
    struct instruction_t instruction = {
        .op = OP_MULTIPLY,
        .value_type = CONSTANT_VALUE_TYPE_NONE,
    };
    vm_write(vm, instruction);
}

void vm_write_divide(struct vm_t* vm) {
    struct instruction_t instruction = {
        .op = OP_DIVIDE,
        .value_type = CONSTANT_VALUE_TYPE_NONE,
    };
    vm_write(vm, instruction);
}

void vm_write_negate(struct vm_t* vm) {
    struct instruction_t instruction = {
        .op = OP_NEGATE,
        .value_type = CONSTANT_VALUE_TYPE_NONE,
    };
    vm_write(vm, instruction);
}

void vm_write_return(struct vm_t* vm) {
    struct instruction_t instruction = {
        .op = OP_RETURN,
        .value_type = CONSTANT_VALUE_TYPE_NONE,
    };
    vm_write(vm, instruction);
}

void vm_write_call(struct vm_t* vm, string name, uint arity) {
    struct instruction_t instruction = {
        .op = OP_CALL,
    };

    u32 index = 0;
    for (uint idx = 0; idx < vm->globals->size; idx++) {
        if (vm->globals->symbols[idx].name.size == name.size && strncmp(vm->globals->symbols[idx].name.chars, name.chars, name.size) == 0) {
            index = vm->globals->symbols[idx].index;
            break;
        }
    }

    instruction.call.name_index = index;
    instruction.call.arity = arity;
    vm_write(vm, instruction);
}

void vm_write_number(struct vm_t* vm, int value) {
    union constant_value_t constant = {
        .number.val = value,
    };
    vm_write_constant(vm, constant);
}

void vm_write_string(struct vm_t* vm, string value) {
    struct instruction_t instruction = {
        .op = OP_CONSTANT,
        .value_type = CONSTANT_VALUE_TYPE_STRING,
        .value = malloc(sizeof(union constant_value_t))
    };
    *instruction.value = (union constant_value_t) {
        .string.val = value,
    };
    vm_write(vm, instruction);
}

void vm_write_symbol(struct vm_t* vm, string name) {
    struct symbol_t symbol = {
        .name = name,
        .index = vm->globals->size,
    };
    vm->globals->symbols[vm->globals->size++] = symbol;
}

void vm_builtin_function(struct vm_t* vm, string name, uint arity, b32 builtin, b32 variadic, void (*func)(void)) {
    vm->globals->symbols[vm->globals->size].name = name;
    vm->globals->symbols[vm->globals->size].index = vm->builtin_func_count;
    vm->builtins[vm->builtin_func_count].name = name;
    vm->builtins[vm->builtin_func_count].arity = arity;
    vm->builtins[vm->builtin_func_count].is_builtin = builtin;
    vm->builtins[vm->builtin_func_count].is_variadic = variadic;
    vm->builtins[vm->builtin_func_count].func = func;
    vm->globals->size++;
    vm->builtin_func_count++;
}
