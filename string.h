#ifndef SOL_STRING_H_INCLUDE_GUARD_
#define SOL_STRING_H_INCLUDE_GUARD_

#include "defs.h"
#include "allocator.h"

typedef struct {
    const char *cstr;
    uint64 len;
} string;

static inline string cstr_to_string(const char *cstr) {
    return (string){cstr, strlen(cstr)};
}

typedef struct {
    // @Unimplemented Temp allocated version of
    // string_buffer for building strings.
} string_builder;

typedef struct {
    char *data;
    uint64_t used;
    uint64_t cap;
    allocator *alloc;
} string_buffer;

string_buffer new_string_buffer(uint64 cap, allocator *alloc);
void free_string_buffer(string_buffer *buf);

string string_buffer_get_string(string_buffer *buf, string *str);
string string_buffer_get_string_from_cstring(string_buffer *buf, const char *cstr);

#endif // include guard
