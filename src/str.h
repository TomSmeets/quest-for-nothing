// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// str.h - simple string handling functions
#pragma once
#include "inc.h"

// Simplicity
// - no 'const' chars, we don't use 'const'

// Number of chars in a zero terminated string
static u32 str_len(char *s) {
    u32 n = 0;
    while (*s++)
        n++;
    return n;
}

static bool str_eq(char *a, char *b) {
    for (;;) {
        if (*a != *b) return 0;
        if (*a == 0) return 1;
        a++;
        b++;
    }
}

static bool str_starts_with(char *str, char *start) {
    for (;;) {
        if (*start == 0) return 1;
        if (*str != *start) return 0;
        str++;
        start++;
    }
}

static bool str_ends_with(char *str, char *end) {
    u32 l1 = str_len(str);
    u32 l2 = str_len(end);
    if (l2 > l1) return 0;
    return str_eq(str + l1 - l2, end);
}

static bool str_chr(char *str, char chr) {
    for(;;) {
        if(*str == 0)   return 0;
        if(*str == chr) return 1;
        str++;
    }
}
