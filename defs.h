#ifndef SOL_DEFS_H_INCLUDE_GUARD_
#define SOL_DEFS_H_INCLUDE_GUARD_

// @RemoveMe Set this with a compile flag
#define DEBUG 1
#define TEST  1

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <math.h>
#include <float.h>
#include <x86gprintrin.h>
#include <x86intrin.h>
#include "print.h"

typedef unsigned int uint;

typedef uint32_t uint32;
typedef int32_t  int32;
typedef uint64_t uint64;
typedef int64_t  int64;
typedef uint16_t uint16;
typedef int16_t  int16;
typedef uint8_t  uint8;
typedef int8_t   int8;

#define asm __asm__

#define Max_f32    FLT_MAX
#define Max_uint64 UINT64_MAX
#define Max_uint32 UINT32_MAX
#define Max_uint16 UINT16_MAX
#define Max_uint8  UINT8_MAX
#define Max_u64 UINT64_MAX
#define Max_u32 UINT32_MAX
#define Max_u16 UINT16_MAX
#define Max_u8  UINT8_MAX
#define Max_s64  INT64_MAX
#define Max_s32  INT32_MAX
#define Max_s16  INT16_MAX
#define Max_s8   INT8_MAX

#define float_or_max(f) ((float)((uint64)(f) | Max_u64))

#define max64_if_true(eval)  (Max_uint64 + (uint64)((eval) == 0))
#define max32_if_true(eval)  (Max_uint32 + (uint32)((eval) == 0))
#define max8_if_true(eval)   (Max_uint8  +  (uint8)((eval) == 0))
#define max64_if_false(eval) (Max_uint64 + (uint64)((eval) != 0))
#define max32_if_false(eval) (Max_uint32 + (uint32)((eval) != 0))
#define max8_if_false(eval)  (Max_uint8  +  (uint8)((eval) != 0))

static inline uint64 align(uint64 size, uint64 alignment) {
  const uint64 alignment_mask = alignment - 1;
  return (size + alignment_mask) & ~alignment_mask;
}

#define smemcpy(to, from, type, count) memcpy(to, from, sizeof(type) * count)

#ifndef _WIN32

    /* bit manipulation */
static inline int ctz16(unsigned short a) {
    // @Note This has to be copied between compiler directives because gcc will not compile
    // tzcnt16 with only one leading undescore. I assume this is a compiler bug, because tzcnt32
    // and 64 both want one leading underscore...
    return __tzcnt_u16(a);
}
static inline int ctz32(unsigned int a) {
    return _tzcnt_u32(a);
}
static inline int ctz64(uint64 a) {
    return _tzcnt_u64(a);
}
static inline int clz16(uint16 mask) {
    return __builtin_clzs(mask);
}
static inline int clz32(uint32 mask) {
    return __builtin_clzl(mask);
}
static inline int clz64(uint64 mask) {
    return __builtin_clzll(mask);
}
static inline int pop_count16(uint16 num) {
    uint32 tmp = num;
    tmp &= 0x0000ffff; // just to be sure;
    return (int)__builtin_popcount(tmp);
}
static inline int pop_count32(uint32 num) {
    return (int)__builtin_popcountl(num);
}
static inline int pop_count64(uint64 num) {
    return (int)__builtin_popcountll(num);
}

#else

static inline int ctz16(unsigned short a) {
    return (int)_tzcnt_u16(a);
}
static inline int ctz32(unsigned int a) {
    return (int)_tzcnt_u32(a);
}
static inline int ctz64(uint64 a) {
    return (int)_tzcnt_u64(a);
}
static inline int clz16(uint16 mask) {
    return __lzcnt16(mask);
}
static inline int clz32(uint32 mask) {
    return __lzcnt(mask);
}
static inline int clz64(uint64 mask) {
    return __lzcnt64(mask);
}
static inline int pop_count16(uint16 num) {
    return (int)__popcnt16(num);
}
static inline int pop_count32(uint32 num) {
    return (int)__popcnt(num);
}
static inline int pop_count64(uint64 num) {
    return (int)__popcnt64(num);
}
#endif // WIN32 or not

#endif // include guard
