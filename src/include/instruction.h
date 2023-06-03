#ifndef INSTRUCTION_H
#define INSTRUCTION_H

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
};

struct instruction_t {
    enum op_t op;
    enum constant_value_type_t value_type;
    union constant_value_t* value;
};

#endif // INSTRUCTION_H
