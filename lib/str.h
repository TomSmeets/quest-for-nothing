// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// str.h - simple string handling functions
#pragma once
#include "types.h"
#include "std.h"

// utf8 encoded string datatype
// String is NOT zero terminated
typedef struct {
    // NOTE: Maximum size is 4 GB (mem chunk is only 1 MB anyway)
    // NOTE: Use 'Text' for a string that can be any size
    u32 len;
    u8 *data;
} String;

// Construct a String from a constant expression
// NOTE: Only use with constant string literals
#define S(x) ((String){sizeof(x) - 1, (u8 *)x})

// A Zero/Null/Empty String
#define S0 ((String){0})

// Number of chars in a zero terminated string
static u32 str_len(char *s) {
    if(!s) return 0;
    u32 n = 0;
    while (*s++) n++;
    return n;
}

// Convert a dynamic zero terminated string to a String
static String str_from(char *data) {
    if(!data) return S0;
    return (String){str_len(data), (u8 *)data};
}

static bool str_eq(String s1, String s2) {
    if (s1.len != s2.len) return false;
    return std_memcmp(s1.data, s2.data, s1.len);
}

static bool strz_eq(char *s1, char *s2) {
    return str_eq(str_from(s1), str_from(s2));
}

// Create a substring
static String str_slice(String str, u32 offset, u32 size) {
    if (offset > str.len) return S0;
    if (offset + size > str.len) size = str.len - offset;
    str.data += offset;
    str.len = size;
    return str;
}

// Take n chars from the start
static String str_take_start(String str, u32 n) {
    if(n > str.len) return str;
    return str_slice(str, 0, n);
}

// Take n chars from the end
static String str_take_end(String str, u32 n) {
    if(n > str.len) return str;
    return str_slice(str, str.len - n, n);
}

// Take n chars from the start
static String str_drop_start(String str, u32 n) {
    if(n >= str.len) return S0;
    return str_slice(str, n, str.len - n);
}

// Take n chars from the end
static String str_drop_end(String str, u32 n) {
    if (n >= str.len) return S0;
    return str_slice(str, 0, str.len - n);
}

static bool str_ends_with(String str, String end) {
    return str_eq(str_take_end(str, end.len), end);
}

static bool str_starts_with(String str, String start) {
    return str_eq(str_take_start(str, start.len), start);
}

static bool str_drop_start_matching(String *str, String start) {
    if (!str_starts_with(*str, start)) return false;
    *str = str_drop_start(*str, start.len);
    return true;
}

static bool str_drop_end_matching(String *str, String end) {
    if (!str_ends_with(*str, end)) return false;
    *str = str_drop_end(*str, end.len);
    return true;
}
