#ifndef CHUNK_H
#define CHUNK_H

#include "types.h"
#include "instruction.h"

struct chunk_t {
    struct instruction_t* instructions;
    uint size;
    uint capacity;
};

#endif // CHUNK_H