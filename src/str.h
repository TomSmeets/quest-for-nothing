// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// str.h - simple string handling functions
#pragma once
#include "types.h"

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
