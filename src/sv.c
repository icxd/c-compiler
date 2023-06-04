#include "include/sv.h"

int sv_compare(struct string_view_t a, struct string_view_t b) {
    if (a.size != b.size) return -1;
    for (unsigned int i = 0; i < a.size; i++) {
        if (a.chars[i] != b.chars[i]) {
            return -1;
        }
    }

    return 0;
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
