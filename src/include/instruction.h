#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "sv.h"
#include "constant.h"

enum op_t {
    OP_STORE_GLOBAL,
    OP_LOAD_GLOBAL,
    OP_CONSTANT,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NEGATE,
    OP_RETURN,
    OP_CALL,
};

struct instruction_t {
    enum op_t op;
    enum constant_value_type_t value_type;
    union constant_value_t* value;
    struct {
        u32 name_index; // Index of the name in the constant pool
        u32 arity; // Number of arguments
    } call;
};

#endif // INSTRUCTION_H
