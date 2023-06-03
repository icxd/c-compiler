#ifndef SV_H
#define SV_H

#include <string.h>

struct string_view_t {
    char* chars;
    unsigned int size;
};

#define SV(str) ((struct string_view_t){ .chars = str, .size = strlen(str) })
#define SV_ARG "%.*s"
#define SV_FMT(sv) (int)(sv).size, (sv).chars

#endif // SV_H
