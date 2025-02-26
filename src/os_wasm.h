// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// os_wasm.h - Platform implementation for wasm
#pragma once
#include "fmt.h"
#include "mem.h"
#include "os.h"
#include "std.h"
#include "str.h"

#define WASM_IMPORT(name) __attribute((import_module("env"), import_name(#name)))

WASM_IMPORT(js_time) u64 js_time(void);
WASM_IMPORT(js_write) void js_write(u8 *data, u32 len);

static OS G_OS;
static Fmt G_FMT;

u64 js_main(void) {
    if (!G->os) {
        G->os = &G_OS;
        G_FMT.out = (void *)1;
        G->fmt = &G_FMT;
    }
    G->os->sleep_time = 1000 * 1000;
    os_main();
    return G->os->sleep_time;
}

static u64 os_time(void) {
    return js_time();
}

static u64 os_rand(void) {
    return js_time();
}

static File *os_stdout(void) {
    return (File *)1;
}

static void os_write(File *file, u8 *data, u32 len) {
    js_write(data, len);
}

static void os_exit(i32 code) {
    __builtin_trap();
}

static void os_fail(char *message) {
    js_write((u8 *)message, str_len(message));
    os_exit(0);
}

#define WASM_PAGE_SIZE 65536

static void *os_alloc_raw(u32 size) {
    u64 addr = __builtin_wasm_memory_size(0) * WASM_PAGE_SIZE;
    __builtin_wasm_memory_grow(0, size / WASM_PAGE_SIZE);
    return (void *)addr;
}

#if 1
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
#endif

static File *os_open(char *path, OS_Open_Type type) {
    os_fail("Platform is not a desktop");
    return 0;
}
static void os_close(File *file) {
    os_fail("Platform is not a desktop");
}

static u32 os_read(File *file, u8 *data, u32 len) {
    os_fail("Platform is not a desktop");
    return 0;
}
static void os_sleep(u64 time) {
    os_fail("Platform is not a desktop");
}

static File *os_dlopen(char *path) {
    os_fail("Platform is not a desktop");
    return 0;
}
static void *os_dlsym(File *handle, char *name) {
    os_fail("Platform is not a desktop");
    return 0;
}
