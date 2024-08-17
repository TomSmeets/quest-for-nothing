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

typedef _Bool bool;
#define true 1
#define false 0

#define static_assert(cond) _Static_assert(cond, "")
#define offset_of(type, field) __builtin_offsetof(type, field)

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

// What is the point of a 8 bit bool?
// Either we care about size or not
//
// Care => u32 bit field
// Not  => u32 for speed
static_assert(sizeof(bool) == 1);
