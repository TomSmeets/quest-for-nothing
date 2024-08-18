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

#define U32_MAX 0xffffffff

// TODO: use bool from c23
typedef _Bool bool;
#define true 1
#define false 0

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

typedef __builtin_va_list va_list;
#define va_start(ap, param) __builtin_va_start(ap, param)
#define va_end(ap) __builtin_va_end(ap)
#define va_arg(ap, type) __builtin_va_arg(ap, type)
