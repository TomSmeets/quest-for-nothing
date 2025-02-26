// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// os_windows.h - Windows API wrapper and platform implementation
#pragma once
#include "fmt.h"
#include "mem.h"
#include "os.h"
#include "std.h"
#include "str.h"
#include <windows.h>

// Export main, allowing us to dynamically call it
__declspec(dllexport) void os_main_dynamic(Global *global_instance) {
    G = global_instance;
    os_main();
}

int main(int argc, char **argv) {
    OS os = {};
    os.argc = argc;
    os.argv = argv;
    G->os = &os;

    Fmt fmt = {};
    fmt.out = GetStdHandle(STD_OUTPUT_HANDLE);
    G->fmt = &fmt;

    Rand rand = rand_new(os_rand());
    G->rand = &rand;
    for (;;) {
        os.sleep_time = 1000 * 1000;
        os_main();
        os_sleep(os.sleep_time);
    }
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

static void os_fail(char *message) {
    fmt_ss(G->fmt, "", message, "\n");

    // check windows error
    DWORD last_error = GetLastError();
    char *last_error_msg = 0;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 0, last_error, 0, (LPTSTR)&last_error_msg, 0, 0);

    fmt_ss(G->fmt, "Windows Error Code: ", last_error_msg, "\n");

    // Exit
    os_exit(1);
}

static void *os_alloc_raw(u32 size) {
    void *alloc = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    assert(alloc, "Failed to allocate memory");
    return alloc;
}

static File *os_open(char *path, OS_Open_Type type) {
    HANDLE handle = 0;
    if (type == Open_Write) {
        handle = CreateFile(path, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    } else if (type == Open_Read) {
        handle = CreateFile(path, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    }

    if (handle == INVALID_HANDLE_VALUE) handle = 0;
    return handle;
}

static void os_close(File *file) {
    CloseHandle(file);
}

static u32 os_read(File *file, u8 *data, u32 len) {
    DWORD bytes_read = 0;
    BOOL success = ReadFile(file, data, len, &bytes_read, 0);
    return bytes_read;
}

static void os_sleep(u64 time) {
    Sleep(time / 1000);
}

static File *os_dlopen(char *path) {
    return (File *)LoadLibrary(path);
}

static void *os_dlsym(File *file, char *name) {
    return GetProcAddress((void *)file, name);
}

static char *os_dlerror(void) {
    return 0;
}

static bool os_system(char *cmd) {
    fmt_ss(G->fmt, "> ", cmd, "\n");
    int ret = system(cmd);
    return ret == 0;
}
