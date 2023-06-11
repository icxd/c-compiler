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

int sv_compare(struct string_view_t a, struct string_view_t b);
int sv_compare_cstring(struct string_view_t a, char* b);
struct string_view_t sv_substr(struct string_view_t sv, unsigned int start, unsigned int end);
char sv_at(struct string_view_t sv, unsigned int index);
struct string_view_t sv_trim(struct string_view_t sv);
struct string_view_t sv_append(struct string_view_t a, struct string_view_t b);

#endif // SV_H
