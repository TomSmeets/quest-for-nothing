// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// str.h - simple string handling functions
#pragma once
#include "os.h"

// Number of chars in a zero terminated string
static u32 str_len(char *s) {
    u32 n = 0;
    while (*s++) n++;
    return n;
}

static bool str_eq(char *s1, char *s2) {
    for (;;) {
        if (*s1 != *s2) return false;
        if (*s1 == 0) return true;
        s1++;
        s2++;
    }
}

static bool str_ends_with(char *str, char *end) {
    u32 l1 = str_len(str);
    u32 l2 = str_len(end);
    if (l2 > l1) return false;
    return str_eq(str + l1 - l2, end);
}

static bool str_starts_with(char *str, char *start) {
    for (;;) {
        if (*start == 0) return true;
        if (*start != *str) return false;
        start++;
        str++;
    }
}
