#ifndef SYMBOL_H
#define SYMBOL_H

#include "types.h"
#include "sv.h"

struct symbol_t {
    string name;
    uint index;
};

struct symbol_table_t {
    struct symbol_t* symbols;
    uint size;
    uint capacity;
};

#endif // SYMBOL_H
