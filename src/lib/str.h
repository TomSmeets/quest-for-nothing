// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// str.h - simple string handling functions
#pragma once
#include "lib/std.h"
#include "lib/types.h"

// utf8 encoded string datatype
// String is NOT zero terminated
typedef struct {
    u32 len;
    bool zero_terminated;
    u8 *data;
} String;

// Construct a String from a constant expression
// NOTE: Only use with constant string literals
#define S(x) ((String){sizeof(x "") - 1, 1, (u8 *)x})

// A Zero/Null/Empty String
#define S0 ((String){0})

// Number of chars in a zero terminated string
static u32 str_len(char *s) {
    if (!s) return 0;
    u32 n = 0;
    while (*s++) n++;
    return n;
}

// Convert a dynamic zero terminated string to a String
static String str_from(char *data) {
    if (!data) return S0;
    return (String){str_len(data), .zero_terminated = 1, (u8 *)data};
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
    if (offset + size < str.len) str.zero_terminated = 0;
    str.data += offset;
    str.len = size;
    return str;
}

// Take n chars from the start
static String str_take_start(String str, u32 n) {
    if (n > str.len) return str;
    return str_slice(str, 0, n);
}

// Take n chars from the end
static String str_take_end(String str, u32 n) {
    if (n > str.len) return str;
    return str_slice(str, str.len - n, n);
}

// Take n chars from the start
static String str_drop_start(String str, u32 n) {
    if (n >= str.len) return S0;
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

// Check if 'str' starts with 'start' and remove it from 'str'
// otherwise return false and keep str unchanged
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

static u32 str_find(String str, u8 chr) {
    for (u32 i = 0; i < str.len; ++i) {
        if (str.data[i] == chr) return i;
    }
    return str.len;
}

// Buffer for storing a c string
typedef struct {
    u8 buffer[1024 * 4];
} CString;

// Quickly convert a 'String' to a zero terminated 'char *'
// If the string is not zero terminated copy to a buffer and zero terminate.
// NOTE: The returned string is only vaild until the next str_c call with the same buffer
// NOTE: string length is limited (See CString)
static char *str_c(CString *cstr, String str) {
    if(str.len == 0) return "";
    if(str.zero_terminated) return (char *) str.data;
    if(str.len >= sizeof(cstr->buffer)) return 0;
    std_memcpy(cstr->buffer, str.data, str.len);
    cstr->buffer[str.len] = 0;
    return (char *)cstr->buffer;
}
