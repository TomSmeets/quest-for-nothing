// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// os_wasm.h - Platform implementation for wasm
#pragma once
#include "os_api.h"
#include "std.h"
#include "types.h"

#define WASM_IMPORT(name) __attribute__((import_module("env"), import_name(#name)))

#if 1
// we need to implement these if we don't use stdlib
// C wil very "helpfully" detect memcpy and memset for loops.
// And translate them to a call to memcpy and memset.
// TODO: Test if this is still the case
void *memcpy(void *restrict dst, const void *restrict src, u64 size) {
    std_memcpy(dst, src, size);
    return dst;
}

void *memset(void *restrict dst, u8 value, u64 size) {
    std_memset(dst, value, size);
    return dst;
}
#endif

WASM_IMPORT(js_write) void js_write(u8 *data, u32 len);
WASM_IMPORT(js_exit) void js_exit(i32 code);
WASM_IMPORT(js_fail) void js_fail(char *message);
WASM_IMPORT(js_alloc_raw) void *js_alloc_raw(u32 size);
WASM_IMPORT(js_time) u64 js_time(void);
WASM_IMPORT(js_sleep) void js_sleep(u64 time);

void js_main(OS *os) {
    os_main(os);
}

static void os_write(u32 fd, u8 *data, u32 len) {
    js_write(data, len);
}

static void os_exit(i32 code) {
    js_exit(code);
}

static void os_fail(char *message) {
    js_fail(message);
}

static void *os_alloc_raw(u32 size) {
    return js_alloc_raw(size);
}

static u64 os_time(void) {
    return js_time();
}

static void os_sleep(u64 time) {
    js_sleep(time);
}
static void *os_load_sdl2(char *name) {
    return 0;
}
