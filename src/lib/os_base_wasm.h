// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// os_base_wasm.h - Base OS implementation for WASM
#pragma once
#include "lib/os_alloc.h"
#include "lib/std.h"
#include "lib/str.h"
#include "lib/types.h"

#if 0
#include "lib/os_base_wasm.js"
#endif

WASM_IMPORT(wasm_time) u64 wasm_time(void);
WASM_IMPORT(wasm_write) void wasm_write(u8 *data, u32 len);

static u64 os_time(void) {
    return wasm_time();
}

static u64 os_rand(void) {
    // TODO: implement a better random
    return wasm_time();
}

static File *os_stdout(void) {
    return (File *)1;
}

static void os_write(File *file, u8 *data, u32 len) {
    wasm_write(data, len);
}

static void os_exit(i32 code) {
    __builtin_trap();
}

// we need to implement these if we don't use stdlib
// C wil very "helpfully" detect memcpy and memset for loops.
// And translate them to a call to memcpy and memset.
// TODO: Test if this is still the case
void *memcpy(void *restrict dst, const void *restrict src, u32 size) {
    std_memcpy(dst, src, size);
    return dst;
}

void *memset(void *restrict dst, u8 value, u32 size) {
    std_memset(dst, value, size);
    return dst;
}
