// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// str_mem.h - String functions that allocate
#pragma once
#include "mem.h"
#include "str.h"

static char *str_dup(char *str, Memory *mem) {
    u32 len = str_len(str);
    char *cpy = mem_push_uninit(mem, len + 1);
    std_memcpy(cpy, str, len + 1);
    return cpy;
}

static char *str_cat3(Memory *mem, char *s0, char *s1, char *s2) {
    u32 l0 = str_len(s0);
    u32 l1 = str_len(s1);
    u32 l2 = str_len(s2);
    char *ret = mem_push_uninit(mem, l0 + l1 + l2);
    std_memcpy(ret, s0, l0);
    std_memcpy(ret + l0, s1, l1);
    std_memcpy(ret + l0 + l1, s2, l2 + 1); // include zero byte
    return ret;
}
