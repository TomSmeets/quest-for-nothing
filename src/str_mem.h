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
