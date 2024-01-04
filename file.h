#ifndef SOL_FILE_H_INCLUDE_GUARD_
#define SOL_FILE_H_INCLUDE_GUARD_

#include "defs.h"
#include "allocator.h"

typedef struct {
    char *data;
    size_t size;
} file;

file file_read_bin_all(const char *file_name, allocator *alloc);
file file_read_char_all(const char *file_name, allocator *alloc);
void file_read_bin_size(const char *file_name, size_t size, void *buffer);
void file_read_char_count(const char *file_name, size_t count, char *buffer);

void file_write_bin(const char *file_name, size_t size, void *data);
void file_write_char(const char *file_name, size_t count, char *data);
void file_append_bin(const char *file_name, size_t size, void *data);
void file_append_char(const char *file_name, size_t count, char *data);

#endif // include guard
