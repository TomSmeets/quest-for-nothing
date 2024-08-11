// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// std.h - Memory copy and zero functions
#pragma once
#include "types.h"

// Append an element to a singly linked list with first and last pointers
#define LIST_APPEND(first, last, el)                                                               \
    do {                                                                                           \
        if ((first)) {                                                                             \
            (last)->next = (el);                                                                   \
            (last) = (el);                                                                         \
        } else {                                                                                   \
            (first) = (last) = (el);                                                               \
        }                                                                                          \
    } while (0)

#define assert(cond, msg)                                                                          \
    if (!(cond)) os_fail(msg "\n")
#define array_count(a) (sizeof(a) / sizeof(a[0]))

typedef __builtin_va_list va_list;
#define va_start(ap, param) __builtin_va_start(ap, param)
#define va_end(ap) __builtin_va_end(ap)
#define va_arg(ap, type) __builtin_va_arg(ap, type)

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
