#ifndef CONSTANT_H
#define CONSTANT_H

#include "sv.h"
#include "types.h"

enum constant_value_type_t {
    CONSTANT_VALUE_TYPE_NONE,
    CONSTANT_VALUE_TYPE_IDX,
    CONSTANT_VALUE_TYPE_NUMBER,
    CONSTANT_VALUE_TYPE_STRING,
};

union constant_value_t {
    struct {
        u32 index;
    } idx;
    struct {
        uint val;
    } number;
    struct {
        string val;
    } string;
};

#endif // CONSTANT_H