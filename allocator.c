#include "allocator.h"
#include "print.h"
#include <stdlib.h>

static heap_allocator fn_new_heap_allocator(uint64 cap, void *buffer);
static linear_allocator fn_new_linear_allocator(uint64 cap, void *buffer);
static void free_heap_allocator(heap_allocator *alloc);
static void free_linear_allocator(linear_allocator *alloc);
static void* malloc_heap(heap_allocator *alloc, uint64 size);
static void* malloc_linear(linear_allocator *alloc, uint64 size);
static void* realloc_heap(heap_allocator *alloc, void *ptr, uint64 new_size);
static void* realloc_linear(linear_allocator *alloc, void *ptr, uint64 new_size);
static void* realloc_linear_with_old_size(linear_allocator *alloc, void *ptr, uint64 old_size, uint64 new_size);
static void free_allocation_heap(heap_allocator *alloc, void *ptr);
static void free_allocation_linear(linear_allocator *alloc, void *ptr); // Does nothing

allocator new_allocator(size_t cap, void *buffer, allocator_flag_bits type)
{
    allocator ret = (allocator){};
    ret.flags = type;

    cap = align(cap, ALLOCATOR_ALIGNMENT);

    if (!buffer)
        buffer = malloc(cap);
    else
        ret.flags |= ALLOCATOR_DO_NOT_FREE_BIT;

    switch(type) {
        case ALLOCATOR_HEAP_BIT:
            ret.heap = fn_new_heap_allocator(cap, buffer);
            break;
        case ALLOCATOR_LINEAR_BIT:
            ret.linear = fn_new_linear_allocator(cap, buffer);
            break;
        default:
            assert(false && "Invalid allocator type");
            break;
    }
    return ret;
}

void free_allocator(allocator *alloc)
{
    if (alloc->flags & ALLOCATOR_DO_NOT_FREE_BIT)
        return;
    switch(alloc->flags & ALLOCATOR_TYPE_BITS) {
    case ALLOCATOR_HEAP_BIT:
        free_heap_allocator(&alloc->heap);
        break;
    case ALLOCATOR_LINEAR_BIT:
        free_linear_allocator(&alloc->linear);
        break;
    default:
        break;
    }
}

void *allocate(allocator *alloc, size_t size)
{
    switch(alloc->flags & ALLOCATOR_TYPE_BITS) {
        case ALLOCATOR_HEAP_BIT:
            return malloc_heap(&alloc->heap, size);
        case ALLOCATOR_LINEAR_BIT:
            return malloc_linear(&alloc->linear, size);
        default:
            assert(false && "Invalid allocator type");
            return NULL;
    }
}

void *reallocate(allocator *alloc, void *ptr, size_t new_size)
{
    switch(alloc->flags & ALLOCATOR_TYPE_BITS) {
        case ALLOCATOR_HEAP_BIT:
            return realloc_heap(&alloc->heap, ptr, new_size);
        case ALLOCATOR_LINEAR_BIT:
            return realloc_linear(&alloc->linear, ptr, new_size);
        default:
            assert(false && "Invalid allocator type");
            return NULL;
    }
}

void *reallocate_with_old_size(allocator *alloc, void *ptr, size_t old_size, size_t new_size)
{
    switch(alloc->flags & ALLOCATOR_TYPE_BITS) {
        case ALLOCATOR_HEAP_BIT:
            return realloc_heap(&alloc->heap, ptr, new_size);
        case ALLOCATOR_LINEAR_BIT:
            return realloc_linear_with_old_size(&alloc->linear, ptr, old_size, new_size);
        default:
            assert(false && "Invalid allocator type");
            return NULL;
    }
}

void deallocate(allocator *alloc, void *ptr)
{
    switch(alloc->flags & ALLOCATOR_TYPE_BITS) {
        case ALLOCATOR_HEAP_BIT:
            return free_allocation_heap(&alloc->heap, ptr);
        case ALLOCATOR_LINEAR_BIT:
            return free_allocation_linear(&alloc->linear, ptr);
        default:
            assert(false && "Invalid allocator type");
            return;
    }
}

static heap_allocator fn_new_heap_allocator(uint64 cap, void *buffer)
{
    heap_allocator ret;
    ret.cap = align(cap, ALLOCATOR_ALIGNMENT);
    ret.mem = buffer;
    ret.used = 0;
    ret.tlsf_handle = tlsf_create_with_pool(ret.mem, ret.cap);
    return ret;
}

static linear_allocator fn_new_linear_allocator(uint64 cap, void *buffer)
{
    linear_allocator ret;
    ret.cap = align(cap, ALLOCATOR_ALIGNMENT);
    ret.mem = buffer;
    ret.used = 0;
    return ret;
}

static void free_heap_allocator(heap_allocator *alloc)
{
    uint64 stats[] = {0, alloc->cap};
    if (alloc->used) {
        pool_t p = tlsf_get_pool(alloc->tlsf_handle);
        tlsf_walk_pool(p, NULL, (void*)&stats);
        println(" Size Remaining In Heap Allocator: %u", alloc->used);
    }

    free(alloc->mem);
    alloc->cap = 0;
}

static void free_linear_allocator(linear_allocator *alloc)
{
    free(alloc->mem);
    alloc->cap = 0;
}

static void* malloc_heap(heap_allocator *alloc, uint64 size)
{
    void *ret = tlsf_memalign(alloc->tlsf_handle, ALLOCATOR_ALIGNMENT, align(size, 16));
    alloc->used += tlsf_block_size(ret);
    return ret;
}

static void* malloc_linear(linear_allocator *alloc, uint64 size)
{
    size = align(size, ALLOCATOR_ALIGNMENT);
    alloc->used = align(alloc->used, ALLOCATOR_ALIGNMENT);
    void *ret = (void*)(alloc->mem + alloc->used);
    alloc->used += size;
    assert(alloc->used <= alloc->cap && "Linear Allocator Overflow");
    return ret;
}

static void* realloc_heap(heap_allocator *alloc, void *ptr, uint64 new_size)
{
    uint64 old_size = tlsf_block_size(ptr);
    alloc->used -= old_size;
    ptr = tlsf_realloc(alloc->tlsf_handle, ptr, align(new_size, 16));
    alloc->used += tlsf_block_size(ptr);
    return ptr;
}

static void* realloc_linear(linear_allocator *alloc, void *ptr, uint64 new_size)
{
    void *p_old = ptr;
    ptr = malloc_linear(alloc, new_size);
    memcpy(ptr, p_old, new_size);
    return ptr;
}

static void* realloc_linear_with_old_size(linear_allocator *alloc, void *ptr, uint64 old_size, uint64 new_size)
{
    void *p_old = ptr;
    ptr = malloc_linear(alloc, new_size);
    memcpy(ptr, p_old, old_size);
    return ptr;
}

static void free_allocation_heap(heap_allocator *alloc, void *ptr)
{
    uint64 size = tlsf_block_size(ptr);
    tlsf_free(alloc->tlsf_handle, ptr);
    alloc->used -= size;
}

void free_allocation_linear(linear_allocator *alloc, void *ptr) { /* Do nothing */ }
