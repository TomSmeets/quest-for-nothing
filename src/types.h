// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// types.h - Basic constant size type definitions
#pragma once

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef signed long long i64;

typedef float f32;
typedef double f64;

typedef f32 v2 __attribute__((ext_vector_type(2)));
typedef f32 v3 __attribute__((ext_vector_type(3)));
typedef f32 v4 __attribute__((ext_vector_type(4)));

typedef u32 v2u __attribute__((ext_vector_type(2)));
typedef u32 v3u __attribute__((ext_vector_type(3)));
typedef u32 v4u __attribute__((ext_vector_type(4)));

typedef i32 v2i __attribute__((ext_vector_type(2)));
typedef i32 v3i __attribute__((ext_vector_type(3)));
typedef i32 v4i __attribute__((ext_vector_type(4)));

// Single 4x4 matrix
// column major, just like OpenGL
// This allows us to extract the columns directly
// | X.x   Y.x   Z.x   W.x |
// |                       |
// | X.y   Y.y   Z.y   W.y |
// |                       |
// | X.z   Y.z   Z.z   W.z |
// |                       |
// | X.w   Y.w   Z.w   W.w |
typedef struct {
    v4 x;
    v4 y;
    v4 z;
    v4 w;
} m4s;

#define U32_MAX 0xffffffff

#if true
#else
typedef _Bool bool;
#define true 1
#define false 0
#endif

#define static_assert(cond) _Static_assert(cond, "")

static_assert(sizeof(u8) == 1);
static_assert(sizeof(u16) == 2);
static_assert(sizeof(u32) == 4);
static_assert(sizeof(u64) == 8);

static_assert(sizeof(i8) == 1);
static_assert(sizeof(i16) == 2);
static_assert(sizeof(i32) == 4);
static_assert(sizeof(i64) == 8);

static_assert(sizeof(f32) == 4);
static_assert(sizeof(f64) == 8);
static_assert(sizeof(bool) == 1);

static_assert(sizeof(v2) == 2 * 4);
static_assert(sizeof(v3) == 4 * 4);
static_assert(sizeof(v4) == 4 * 4);

static_assert(sizeof(v2i) == 2 * 4);
static_assert(sizeof(v3i) == 4 * 4);
static_assert(sizeof(v4i) == 4 * 4);

static_assert(sizeof(v2u) == 2 * 4);
static_assert(sizeof(v3u) == 4 * 4);
static_assert(sizeof(v4u) == 4 * 4);

#define offset_of(type, field) __builtin_offsetof(type, field)
#define array_count(a) (sizeof(a) / sizeof(a[0]))

typedef __builtin_va_list va_list;
#define va_start(ap, param) __builtin_va_start(ap, param)
#define va_end(ap) __builtin_va_end(ap)
#define va_arg(ap, type) __builtin_va_arg(ap, type)
