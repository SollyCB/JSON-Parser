#include "file.h"

file file_read_bin_all(const char *file_name, allocator *alloc)
{
    FILE *f = fopen(file_name, "rb");
    if (!f) {
        println("FILE: Failed to open file '%s'", file_name);
        return (file){};
    }
    fseek(f, 0, SEEK_END);
    file ret;
    ret.size = ftell(f);
    ret.data = allocate(alloc, ret.size);
    fread(ret.data, 1, ret.size, f);
    fclose(f);
    return ret;
}

file file_read_char_all(const char *file_name, allocator *alloc)
{
    FILE *f = fopen(file_name, "r");
    if (!f) {
        println("FILE: Failed to open file '%s'", file_name);
        return (file){};
    }
    fseek(f, 0, SEEK_END);
    file ret;
    ret.size = ftell(f);
    ret.data = allocate(alloc, ret.size);
    fseek(f, 0, SEEK_SET);
    fread(ret.data, 1, ret.size, f);
    fclose(f);
    return ret;
}

void file_read_bin_size(const char *file_name, size_t size, void *buffer)
{
    FILE *f = fopen(file_name, "rb");
    if (!f) {
        println("FILE: Failed to open file '%s'", file_name);
        return;
    }
    fread(buffer, 1, size, f);
    fclose(f);
}

void file_read_char_count(const char *file_name, size_t count, char *buffer)
{
    FILE *f = fopen(file_name, "r");
    if (!f) {
        println("FILE: Failed to open file '%s'", file_name);
        return;
    }
    fread(buffer, 1, count, f);
    fclose(f);
}

void file_write_bin(const char *file_name, size_t size, void *data)
{
    FILE *f = fopen(file_name, "wb");
    if (!f) {
        println("FILE: Failed to open file '%s'", file_name);
        return;
    }
    fwrite(data, 1, size, f);
    fclose(f);
}

void file_write_char(const char *file_name, size_t count, char *data)
{
    FILE *f = fopen(file_name, "w");
    if (!f) {
        println("FILE: Failed to open file '%s'", file_name);
        return;
    }
    fwrite(data, 1, count, f);
    fclose(f);
}

void file_append_bin(const char *file_name, size_t size, void *data)
{
    FILE *f = fopen(file_name, "ab");
    if (!f) {
        println("FILE: Failed to open file '%s'", file_name);
        return;
    }
    fwrite(data, 1, size, f);
    fclose(f);
}

void file_append_char(const char *file_name, size_t count, char *data)
{
    FILE *f = fopen(file_name, "a");
    if (!f) {
        println("FILE: Failed to open file '%s'", file_name);
        return;
    }
    fwrite(data, 1, count, f);
    fclose(f);
}
