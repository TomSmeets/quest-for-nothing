// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// str_mem.h - String functions that allocate
#pragma once
#include "mem.h"
#include "str.h"

// Allocate a new and empty string
static String str_alloc(Memory *mem, u32 len) {
    String str = {
        .len = len,
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

// Copy string and include zero terminator
static char *str_to_c(Memory *mem, String str) {
    char *cstr = mem_push_uninit(mem, str.len + 1);
    std_memcpy((u8 *)cstr, str.data, str.len);
    cstr[str.len] = 0;
    return cstr;
}

static u8 str_buf[1024];

// Copy string and include zero terminator
// Quickly convert a 'String' to a zero terminated 'char *'
// NOTE: this pointer is only valid until the next call! And not at all thread safe!
static char *str_c(String str) {
    assert(str.len < sizeof(str_buf), "String out of range");
    std_memcpy(str_buf, str.data, str.len);
    str_buf[str.len] = 0;
    return (char *) str_buf;
}

static void test_str(void) {
    Memory *mem = mem_new();

    // `S0` and `S()`
    assert(str_eq(S0, S0) == 1, "Zero string is equal to itself");
    assert(str_eq(S(""), S0) == 1, "Any empty string is equal to the zero string");
    assert(str_eq(S(""), S("")) == 1, "All empty strings are equal");
    assert(str_eq(S("A"), S0) == 0, "Non-empty strings are not equal to empty strings");
    assert(str_eq(S0, S("A")) == 0, "Non-empty strings are not equal to empty strings (reverse order)");
    assert(str_eq(S("A"), S("A")) == 1, "Strings can be equal");
    assert(str_eq(S("Hello"), S("World")) == 0, "Differfent strings should not be equal");
    assert(str_eq(S("Hello"), S("Hello")) == 1, "Same strings should be equal");

    // str_eq()
    String s_hello = S("Hello ");
    String s_world = S("World!");
    assert0(str_eq(s_hello, s_hello) == true);
    assert0(str_eq(s_world, s_world) == true);
    assert0(str_eq(s_hello, s_world) == false);
    assert0(str_eq(s_world, s_hello) == false);

    // str_slice()
    assert0(str_eq(str_slice(S("ABCDEF"), 0, 6), S("ABCDEF")));
    assert0(str_eq(str_slice(S("ABCDEF"), 0, 3), S("ABC")));
    assert0(str_eq(str_slice(S("ABCDEF"), 3, 3), S("DEF")));
    assert0(str_eq(str_slice(S("ABCDEF"), 2, 1), S("C")));

    // str_starts_with()
    assert0(str_ends_with(S("ABCDEF"), S("F")) == 1);
    assert0(str_ends_with(S("ABCDEF"), S("DEF")) == 1);
    assert0(str_ends_with(S("ABCDEF"), S("ABCDEF")) == 1);
    assert0(str_ends_with(S("ABCDEF"), S("XABCDEF")) == 0);
    assert0(str_ends_with(S("ABCDEF"), S("ABCDEFX")) == 0);
    assert0(str_ends_with(S("ABCDEF"), S("X")) == 0);
    assert0(str_ends_with(S("ABCDEF"), S("")) == 1);
    assert0(str_ends_with(S(""), S("")) == 1);
    assert0(str_ends_with(S(""), S("A")) == 0);

    // str_ends_with()
    assert0(str_starts_with(S("ABCDEF"), S("A")) == 1);
    assert0(str_starts_with(S("ABCDEF"), S("ABC")) == 1);
    assert0(str_starts_with(S("ABCDEF"), S("ABCDEF")) == 1);
    assert0(str_starts_with(S("ABCDEF"), S("ABCDEFX")) == 0);
    assert0(str_starts_with(S("ABCDEF"), S("XABCDEF")) == 0);
    assert0(str_starts_with(S("ABCDEF"), S("X")) == 0);
    assert0(str_starts_with(S("ABCDEF"), S("")) == 1);
    assert0(str_starts_with(S(""), S("")) == 1);
    assert0(str_starts_with(S(""), S("A")) == 0);

    // str_take() / str_drop()
    assert(str_eq(str_take_start(S("Hello World!"), 5), S("Hello")), "Take Start");
    assert(str_eq(str_take_end(S("Hello World!"), 6), S("World!")), "Take End");
    assert(str_eq(str_drop_start(S("Hello World!"), 6), S("World!")), "Drop Start");
    assert(str_eq(str_drop_end(S("Hello World!"), 7), S("Hello")), "Drop End");

    // str_append()
    String s_greeting = str_append(mem, s_hello, s_world);
    assert(str_eq(s_hello, S("Hello ")), "Original should not be changed");
    assert(str_eq(s_world, S("World!")), "Original should not be changed");
    assert(str_eq(s_greeting, S("Hello World!")), "Strings should be concatinated");

    // str_replace()
    assert(str_eq(str_replace(mem, S("Hello test World!"), 6, 4, S("AWESOME")), S("Hello AWESOME World!")), "Replace test");
    assert(str_eq(str_replace(mem, S("Hello test World!"), 6, 5, S0), S("Hello World!")), "Delete test");
    assert(str_eq(str_replace(mem, S("Hello test World!"), 0, 6, S0), S("test World!")), "Delete string-start");
    assert(str_eq(str_replace(mem, S("Hello test World!"), 0, 17, S0), S0), "Delte everything");
    assert(str_eq(str_replace(mem, S0, 0, 0, S0), S0), "Edge case");

    // str_clone()
    String s7 = str_clone(mem, s_hello);
    assert0(str_eq(s7, s_hello));
    assert0(s7.data != s_hello.data);

    mem_free(mem);
}
