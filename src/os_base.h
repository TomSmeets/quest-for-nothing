// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// os.h - Base platform API
#pragma once
#include "os_alloc.h"
#include "os_fail.h"
#include "std.h"
#include "str.h"
#include "types.h"

typedef struct File File;

static u64 os_time(void);
static void os_write(File *file, u8 *data, u32 len);
static void os_exit(i32 code);
static void *os_alloc_raw(u32 size);

#if OS_IS_LINUX
#include "linux_api.h"

static u64 os_time(void) {
    struct linux_timespec t = {};
    linux_clock_gettime(CLOCK_MONOTONIC, &t);
    return linux_time_to_us(&t);
}

static void os_write(File *file, u8 *data, u32 len) {
    u32 written = 0;
    while (written < len) {
        i64 result = linux_write(fd_from_file(file), data + written, len - written);
        assert(result > 0, "Failed to write");
        written += result;
    }
}

static void os_exit(i32 status) {
    linux_exit_group(status);
}

static void *os_alloc_raw(u32 size) {
    i32 prot = PROT_READ | PROT_WRITE;
    i32 flags = MAP_PRIVATE | MAP_ANONYMOUS;
    void *alloc = linux_mmap(0, size, prot, flags, -1, 0);
    assert(alloc && alloc != MAP_FAILED, "Failed to allocate memory");
    return alloc;
}

#elif OS_IS_WINDOWS
static u64 os_time(void) {
    LARGE_INTEGER big_freq, big_count;
    assert(QueryPerformanceFrequency(&big_freq), "Failed to get performance frequency");
    assert(QueryPerformanceCounter(&big_count), "Failed to get performance counter");
    i64 freq = big_freq.QuadPart;
    i64 count = big_count.QuadPart;
    assert(freq >= 1000 * 1000, "Invalid performance frequency");
    assert(count >= 0, "Invalid performance counter");
    return (u64)count / ((u64)freq / 1000 / 1000);
}

static u64 os_rand(void) {
    // A crappy rng, but fine for now
    return os_time();
}

static void os_write(File *file, u8 *data, u32 len) {
    WriteFile(file, data, len, 0, 0);
}

static void os_exit(i32 code) {
    ExitProcess(code);
}

static void windows_print(char *message) {
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteFile(out, message, str_len(message), 0, 0);
}

static void *os_alloc_raw(u32 size) {
    void *alloc = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    assert(alloc, "Failed to allocate memory");
    return alloc;
}

#elif OS_IS_WASM
WASM_IMPORT(js_time) u64 js_time(void);
WASM_IMPORT(js_write) void js_write(u8 *data, u32 len);

static u64 os_time(void) {
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
