// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
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

// File handle
typedef struct File File;

#define U32_MAX 0xffffffff

// Define bool for clang-format
#if __STDC_VERSION__ <= 201710L
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

#define offset_of(type, field) __builtin_offsetof(type, field)
#define array_count(a) (sizeof(a) / sizeof(a[0]))

#define GUARD(x)                                                                                                                                     \
    if (!(x)) continue
