// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// str_mem.h - String functions that allocate
#pragma once
#include "lib/mem.h"
#include "lib/str.h"

// Allocate a new and empty string
static String str_alloc(Memory *mem, u32 len) {
    String str = {
        .len = len,
        .zero_terminated = 1,
        .data = mem_push_uninit(mem, len + 1),
    };
    str.data[str.len] = 0;
    return str;
}

// Delte chars from `pos` until `pos + del` and replace it with `new`
// The result is stored in a new allocated String, the original string is not modified
static String str_replace(Memory *mem, String str, u32 pos, u32 del, String new) {
    assert(str.len >= pos + del, "Out of range");
    String str2 = str_alloc(mem, str.len - del + new.len);
    std_memcpy(str2.data + 0, str.data, pos);
    std_memcpy(str2.data + pos, new.data, new.len);
    std_memcpy(str2.data + pos + new.len, str.data + pos + del, str.len - pos - del);
    return str2;
}

// Append two strings
// The result is stored in a new allocated String, the original string is not modified
static String str_append(Memory *mem, String s1, String s2) {
    String ret = str_alloc(mem, s1.len + s2.len);
    std_memcpy(ret.data, s1.data, s1.len);
    std_memcpy(ret.data + s1.len, s2.data, s2.len);
    return ret;
}

// Append two strings
// The result is stored in a new allocated String, the original string is not modified
static String str_cat3(Memory *mem, String s1, String s2, String s3) {
    String ret = str_alloc(mem, s1.len + s2.len + s3.len);
    std_memcpy(ret.data, s1.data, s1.len);
    std_memcpy(ret.data + s1.len, s2.data, s2.len);
    std_memcpy(ret.data + s1.len + s2.len, s3.data, s3.len);
    return ret;
}

// Allocate a new copy of `str`
static String str_clone(Memory *mem, String str) {
    String new = str_alloc(mem, str.len);
    std_memcpy(new.data, str.data, str.len);
    return new;
}

// Quickly convert a 'String' to a zero terminated 'char *'
// Either returns the original string or reallocates to G->tmp with a zero byte
static char *str_c(String str) {
    if (str.len == 0) return "";
    if (str.zero_terminated) return (char *)str.data;
    char *cstr = mem_push_uninit(G->tmp, str.len + 1);
    std_memcpy((u8 *)cstr, str.data, str.len);
    cstr[str.len] = 0;
    return cstr;
}
