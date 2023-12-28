// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// inc.h - defines various types and macros that are used everywhere
#pragma once

// typedef all structs beforehand. I don't like adding typedef struct, so I use this
#ifdef BOOTSTRAP
#include "generated-small.h"
#else
#include "generated.h"
#endif

// Simplicity
// every small file should be reusable
// everything static and single header files
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed int i32;
typedef unsigned long long u64;
typedef signed long long i64;
typedef float f32;
typedef _Bool bool;

#define static_assert(x) _Static_assert(x, #x)
#define array_count(a) (sizeof(a) / sizeof(a[0]))

static_assert(sizeof(u8)  == 1);
static_assert(sizeof(u16) == 2);
static_assert(sizeof(u32) == 4);
static_assert(sizeof(u64) == 8);
static_assert(sizeof(i32) == 4);
static_assert(sizeof(i64) == 8);
static_assert(sizeof(f32) == 4);

#define U64_MAX 0xffffffffffffffff
#define U32_MAX 0xffffffff
#define U16_MAX 0xffff

#define SIZE_KB (1024ULL)
#define SIZE_MB (1024ULL * 1024ULL)
#define SIZE_GB (1024ULL * 1024ULL * 1024ULL)

#define guard(c) if (!(c)) continue
#define tostring0(x) #x
#define tostring1(x) tostring0(x)

#define assert(cond) os_assert(cond, __FILE__ ":" tostring1(__LINE__) ": Assertion failed assert(" #cond ")\n")
static void os_assert(bool cond, char *msg);

struct buf {
    void *ptr;
    u64 size;
};

// Simple wrappers for all compiler builtins,
static void std_memcpy(void *restrict dst, const void *restrict src, u64 size) {
    while (size--)
        *(u8 *)dst++ = *(u8 *)src++;
}

static void std_memset(void *restrict dst, u8 c, u64 size) {
    while (size--)
        *(u8 *)dst++ = c;
}

static void std_memzero(void *restrict dst, u64 size) {
    while (size--)
        *(u8 *)dst++ = 0;
}

static void std_reverse(u64 size, u8 *buf) {
    for (u32 i = 0; i < size / 2; ++i) {
        u8 c = buf[i];
        buf[i] = buf[size - i - 1];
        buf[size - i - 1] = c;
    }
}
