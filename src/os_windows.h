// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// os_windows.h - Windows API wrapper and platform implementation
#pragma once
#include "os_api.h"
#include "std.h"
#include "str.h"
#include <windows.h>

static void os_write(u8 *msg, u32 len) {
    HANDLE con = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteFile(con, msg, len, 0, 0);
}

static void os_exit(i32 code) {
    ExitProcess(code);
}

static void os_fail(char *msg) {
    HANDLE con = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteFile(con, msg, str_len(msg), 0, 0);

    // check windows error
    DWORD last_error = GetLastError();
    char *last_error_msg = 0;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 0, last_error, 0,
        (LPTSTR)&last_error_msg, 0, 0
    );
    WriteFile(con, last_error_msg, str_len(last_error_msg), 0, 0);

    // Exit
    ExitProcess(1);
}

static void *os_alloc_raw(u32 size) {
    void *alloc = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    assert(alloc, "Failed to allocate memory");
    return alloc;
}

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

static void os_sleep(u64 time) {
    Sleep(time / 1000);
}

static void *os_load_sdl2(char *name) {
    OS *os = OS_GLOBAL;

    if (!os->sdl2_handle) {
        os->sdl2_handle = LoadLibrary("SDL2.dll");
        assert(os->sdl2_handle, "Failed to load SDL2.dll");
    }

    return GetProcAddress(os->sdl2_handle, name);
}

// Export main, allowing us to dynamically call it
void os_main_dynamic(OS *os) {
    OS_GLOBAL = os;
    os_main(os);
}

int main(int argc, const char **argv) {
    OS os = {};
    os.argc = argc;
    os.argv = (char **)argv;
    for (;;) {
        os_main_dynamic(&os);
    }
}
