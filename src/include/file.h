#ifndef FILE_H
#define FILE_H

#include "sv.h"
#include "types.h"

struct file_t {
    string path;
    string contents;
};

struct file_t* file_new(string path);
void file_free(struct file_t* file);
string file_read_to_string(string path);

#endif // FILE_H
