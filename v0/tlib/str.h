// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// str.h - simple string handling functions
#pragma once
#include "tlib/inc.h"
#include "tlib/mem.h"

// Simplicity
// - no 'const' chars, we don't use 'const'

// Number of chars in a zero terminated string
static u32 str_len(char *s) {
    u32 n = 0;
    while (*s++)
        n++;
    return n;
}

// find a character in the string
static u32 str_find(char *str, char find) {
    u32 n = 0;
    for (;;) {
        char c = str[n];
        if (c == find || c == 0)
            return n;
        n++;
    }
}

static bool str_eq(char *a, char *b) {
    for (;;) {
        if (*a != *b)
            return 0;
        if (*a == 0)
            return 1;
        a++;
        b++;
    }
}

static bool str_starts_with(char *str, char *start) {
    for (;;) {
        if (*start == 0)
            return 1;
        if (*str != *start)
            return 0;
        str++;
        start++;
    }
}

static bool str_ends_with(char *str, char *end) {
    u32 l1 = str_len(str);
    u32 l2 = str_len(end);
    if (l2 > l1)
        return 0;
    return str_eq(str + l1 - l2, end);
}

static bool str_chr(char *str, char chr) {
    for (;;) {
        if (*str == 0)
            return 0;
        if (*str == chr)
            return 1;
        str++;
    }
}

// ==== Using Memory allocation ====
static char *str_dup_len(mem *m, char *str, u32 len) {
    char *out = mem_push(m, len + 1);
    std_memcpy(out, str, len);
    out[len] = 0;
    return out;
}

static char *str_dup(mem *m, char *str) {
    return str_dup_len(m, str, str_len(str));
}

static bool str_contains(char *str, char *contained) {
    for (;;) {
        if (*str == 0)
            return 0;
        if (str_starts_with(str, contained))
            return 1;
        str++;
    }
}
