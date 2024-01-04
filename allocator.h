#ifndef SOL_ALLOCATOR_H_INCLUDE_GUARD_
#define SOL_ALLOCATOR_H_INCLUDE_GUARD_

#include "external/tlsf.h"
#include "defs.h"
#include "assert.h"

#define ALLOCATOR_ALIGNMENT 16

typedef struct {
    void *data;
    size_t size;
} allocation;

typedef struct {
    uint64 cap;
    uint64 used;
    uint8 *mem;
    void *tlsf_handle;
} heap_allocator;

typedef struct {
    uint64 cap;
    uint64 used;
    uint8 *mem;
} linear_allocator;

typedef enum {
    ALLOCATOR_HEAP_BIT        = 0x01,
    ALLOCATOR_LINEAR_BIT      = 0x02,
    ALLOCATOR_DO_NOT_FREE_BIT = 0x04,

    ALLOCATOR_TYPE_BITS = ALLOCATOR_HEAP_BIT | ALLOCATOR_LINEAR_BIT,
} allocator_flag_bits;

typedef struct {
    uint32 flags;
    union {
        heap_allocator heap;
        linear_allocator linear;
    };
} allocator;

// Leave 'buffer' null to allocate cap from standard allocator.
allocator new_allocator(size_t cap, void *buffer, allocator_flag_bits type);
void free_allocator(allocator *alloc);

#define new_heap_allocator(cap, buffer) new_allocator(cap, buffer, ALLOCATOR_HEAP_BIT)
#define new_linear_allocator(cap, buffer) new_allocator(cap, buffer, ALLOCATOR_LINEAR_BIT)

void *allocate(allocator *alloc, size_t size);
void *reallocate(allocator *alloc, void *ptr, size_t new_size);
void *reallocate_with_old_size(allocator *alloc, void *ptr, size_t old_size, size_t new_size);
void deallocate(allocator *alloc, void *ptr);

#define sallocate(alloc, type, count) allocate(alloc, sizeof(type) * count)
#define sreallocate(alloc, ptr, type, count) reallocate(alloc, ptr, sizeof(type) * count)

static inline size_t allocator_used(allocator *alloc) {
    switch(alloc->flags & ALLOCATOR_TYPE_BITS) {
    case ALLOCATOR_HEAP_BIT:
        return alloc->heap.used;
    case ALLOCATOR_LINEAR_BIT:
        return alloc->linear.used;
    default:
        assert(false && "Invalid allocator flags");
        return Max_u64;
    }
}

static inline void allocator_reset_linear(allocator *alloc) {
    if (alloc->flags & ALLOCATOR_LINEAR_BIT)
        alloc->linear.used = 0;
}

static inline void allocator_reset_linear_to(allocator *alloc, uint64 to) {
    assert(to < alloc->linear.used);
    if (alloc->flags & ALLOCATOR_LINEAR_BIT)
        alloc->linear.used = to;
}

#endif // include guard
