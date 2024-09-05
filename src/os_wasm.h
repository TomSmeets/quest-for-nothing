// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// os_wasm.h - Platform implementation for wasm
#pragma once
#include "os_api.h"
#include "std.h"
#include "str.h"
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
WASM_IMPORT(js_exit) void js_exit(void);
WASM_IMPORT(js_time) u64 js_time(void);

static OS JS_OS;

u64 js_main(void) {
    os_main(&JS_OS);
    return JS_OS.sleep_time;
}

static void os_write(u32 fd, u8 *data, u32 len) {
    js_write(data, len);
}

static void os_exit(i32 code) {
    js_exit();
}

static void os_fail(char *message) {
    js_write((u8*)message, str_len(message));
    js_exit();
}

#define WASM_PAGE_SIZE 65536

static void *os_alloc_raw(u32 size) {
    u64 addr = __builtin_wasm_memory_size(0) * WASM_PAGE_SIZE;
    __builtin_wasm_memory_grow(0, size / WASM_PAGE_SIZE);
    os_print("Memory Alloc!\n");
    return (void*) addr;
}

static u64 os_time(void) {
    os_print("Time!\n");
    return js_time();
}

static void os_sleep(u64 time) { }

static void *os_load_sdl2(char *name) {
    os_print("SDL!\n");
    os_print(name);
    return 0;
}
