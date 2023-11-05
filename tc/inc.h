#pragma once

// Simplicity
// every small file should be reusable
typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;
typedef   signed int       i32;
typedef   signed long long i64;
typedef float f32;
typedef u32 b32;

#define U32_MAX 0xffffffff

#define guard(c) if(!(c)) continue
#define tostring0(x)  #x
#define tostring1(x) tostring0(x)
#define assert(cond) do { if(!(cond)) os_fail(__FILE__, tostring1(__LINE__), (char *) __func__, #cond); } while(0)
