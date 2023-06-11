#include <string.h>
#include <stdlib.h>

#include "include/sv.h"

int sv_compare(struct string_view_t a, struct string_view_t b) {
    if (a.size != b.size) return 0;
    for (unsigned int i = 0; i < a.size; i++) {
        if (a.chars[i] != b.chars[i]) {
            return 0;
        }
    }

    return 1;
}

int sv_compare_cstring(struct string_view_t a, char* b) {
    return sv_compare(a, SV(b));
}

struct string_view_t sv_substr(struct string_view_t sv, unsigned int start, unsigned int end) {
    if (start > sv.size || end > sv.size)
        return (struct string_view_t){ .chars = NULL, .size = 0 };
    return (struct string_view_t){ .chars = sv.chars + start, .size = end - start };
}

char sv_at(struct string_view_t sv, unsigned int index) {
    if (index > sv.size) return '\0';
    return sv.chars[index];
}

struct string_view_t sv_trim(struct string_view_t sv) {
    unsigned int start = 0;
    unsigned int end = sv.size - 1;

    while (sv.chars[start] == ' ' || sv.chars[start] == '\t') start++;
    while (sv.chars[end] == ' ' || sv.chars[end] == '\t') end--;

    return sv_substr(sv, start, end + 1);
}

struct string_view_t sv_append(struct string_view_t a, struct string_view_t b) {
    char* new_chars = malloc(a.size + b.size + 1);
    memcpy(new_chars, a.chars, a.size);
    memcpy(new_chars + a.size, b.chars, b.size);
    new_chars[a.size + b.size] = '\0';
    return (struct string_view_t){ .chars = new_chars, .size = a.size + b.size };
}
