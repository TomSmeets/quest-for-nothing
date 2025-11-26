// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// types.h - Basic constant size type definitions
#pragma once

// Constants
#define U32_MAX 0xffffffff

// Helper macros
#define static_assert(cond) _Static_assert(cond, "")
#define offset_of(type, field) __builtin_offsetof(type, field)
#define array_count(a) (sizeof(a) / sizeof(a[0]))

// Base types
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
static_assert(sizeof(u8) == 1);
static_assert(sizeof(u16) == 2);
static_assert(sizeof(u32) == 4);
static_assert(sizeof(u64) == 8);

typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef signed long long i64;
static_assert(sizeof(i8) == 1);
static_assert(sizeof(i16) == 2);
static_assert(sizeof(i32) == 4);
static_assert(sizeof(i64) == 8);

typedef float f32;
typedef double f64;
static_assert(sizeof(f32) == 4);
static_assert(sizeof(f64) == 8);

#if __STDC_VERSION__ <= 201710L && !__cplusplus
typedef _Bool bool;
#define true 1
#define false 0
#endif
static_assert(sizeof(bool) == 1);

#define GUARD(x)                                                                                                                                     \
    if (!(x)) continue

#define if_not(x) if (!(x))

typedef struct File File;
