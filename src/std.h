// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// std.h - Memory copy and zero functions
#pragma once
#include "os_api.h"

// Append an element to a singly linked list with first and last pointers
#define LIST_APPEND(first, last, el)                                                                                                                 \
    do {                                                                                                                                             \
        if ((first)) {                                                                                                                               \
            (last)->next = (el);                                                                                                                     \
            (last) = (el);                                                                                                                           \
        } else {                                                                                                                                     \
            (first) = (last) = (el);                                                                                                                 \
        }                                                                                                                                            \
    } while (0)

#define assert(cond, msg) if (!(cond)) os_fail(msg "\n")

static void std_memcpy(void *restrict dst, const void *restrict src, u64 size) {
    while (size--)
        *(u8 *)dst++ = *(u8 *)src++;
}

static void std_memzero(void *restrict dst, u64 size) {
    while (size--)
        *(u8 *)dst++ = 0;
}

static void std_memset(void *restrict dst, u8 value, u64 size) {
    while (size--)
        *(u8 *)dst++ = value;
}

static void std_reverse(u8 *buf, u32 size) {
    for (u32 i = 0; i < size / 2; ++i) {
        u8 c = buf[i];
        buf[i] = buf[size - i - 1];
        buf[size - i - 1] = c;
    }
}
