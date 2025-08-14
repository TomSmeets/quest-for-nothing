// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// std.h - Memory copy and zero functions
#pragma once
#include "lib/types.h"

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

// Append an element to a singly linked list with first and last pointers
#define LIST_APPEND2(FIRST, LAST, EL, NEXT)                                                                                                          \
    do {                                                                                                                                             \
        if ((FIRST)) {                                                                                                                               \
            (LAST)->NEXT = (EL);                                                                                                                     \
            (LAST) = (EL);                                                                                                                           \
        } else {                                                                                                                                     \
            (FIRST) = (LAST) = (EL);                                                                                                                 \
        }                                                                                                                                            \
    } while (0)

static void std_memcpy(u8 *restrict dst, const u8 *restrict src, u32 size) {
    while (size--) *dst++ = *src++;
}

static void std_memmove(u8 *dst, const u8 *src, u32 size) {
    while (size--) *dst++ = *src++;
}

static void std_memzero(u8 *dst, u32 size) {
    while (size--) *dst++ = 0;
}

static void std_memset(u8 *dst, u8 value, u32 size) {
    while (size--) *dst++ = value;
}

static bool std_memcmp(const u8 *restrict a, const u8 *restrict b, u32 size) {
    while (size--) {
        if (*a++ != *b++) return false;
    }
    return true;
}

static void std_reverse(u8 *buf, u32 size) {
    for (u32 i = 0; i < size / 2; ++i) {
        u8 c = buf[i];
        buf[i] = buf[size - i - 1];
        buf[size - i - 1] = c;
    }
}

// Return number of bytes needed to achive the alignment
static u32 std_align_offset(void *ptr, u32 align) {
    u32 mask = align - 1;
    return ((u64)ptr + mask) & mask - (u64)ptr;
}
