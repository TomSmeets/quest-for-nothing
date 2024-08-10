// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// str.h - simple string handling functions
#pragma once
#include "types.h"

// Number of chars in a zero terminated string
static u32 str_len(char *s) {
    u32 n = 0;
    while (*s++)
        n++;
    return n;
}
