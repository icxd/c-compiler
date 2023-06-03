#ifndef VM_H
#define VM_H

#include "sv.h"
#include "types.h"
#include "symbol.h"
#include "chunk.h"
#include "instruction.h"
#include "constant.h"

struct vm_t {
    struct chunk_t* chunk;
    
    uint ip;
    
    struct symbol_table_t* globals;
    
    uint stack[256];
    uint stack_size;
    
    uint builtin_func_count;
};

struct vm_t* vm_new(void);
void vm_free(struct vm_t* vm);
void vm_write(struct vm_t* vm, struct instruction_t instruction);
void vm_write_constant(struct vm_t* vm, union constant_value_t value);
void vm_write_store_global(struct vm_t* vm, string name);
void vm_write_load_global(struct vm_t* vm, string name);
void vm_write_add(struct vm_t* vm);
void vm_write_subtract(struct vm_t* vm);
void vm_write_multiply(struct vm_t* vm);
void vm_write_divide(struct vm_t* vm);
void vm_write_negate(struct vm_t* vm);
void vm_write_return(struct vm_t* vm);
void vm_write_call(struct vm_t* vm, string name, uint arity);
void vm_write_number(struct vm_t* vm, int value);
void vm_write_string(struct vm_t* vm, string value);
void vm_write_symbol(struct vm_t* vm, string name);

#endif // VM_H
