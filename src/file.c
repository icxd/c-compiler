#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "include/sv.h"
#include "include/types.h"
#include "include/file.h"

struct file_t* file_new(string path) {
    struct file_t* file = malloc(sizeof(struct file_t));
    file->path = path;
    file->contents = read_file_to_string(path);
    return file;
}

void file_free(struct file_t* file) {
    free(file);
}

string read_file_to_string(string path) {
    FILE* filepoint;
    errno_t err;

    if ((err = fopen_s(&filepoint, path.chars, "r")) != 0) {
        fprintf(stderr, "Failed to open file '"SV_ARG"' for reading.\n", SV_FMT(path));
        exit(1);
    } else {
        fseek(filepoint, 0, SEEK_END);
        long size = ftell(filepoint);
        fseek(filepoint, 0, SEEK_SET);

        char* raw_contents = malloc(size + 1);
        fread(raw_contents, 1, size, filepoint);
        raw_contents[size] = '\0';

        fclose(filepoint);
        return SV(raw_contents);
    }
}
