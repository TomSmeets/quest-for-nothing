// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// os.h - Generic platform API
#pragma once
#include "os_api.h"

#if __unix__
#define OS_IS_LINUX 1
#else
#define OS_IS_LINUX 0
#endif

#if _WIN32
#define OS_IS_WINDOWS 1
#else
#define OS_IS_WINDOWS 0
#endif

#if __wasm__
#define OS_IS_WASM 1
#else
#define OS_IS_WASM 0
#endif

#if OS_IS_LINUX
#include "os_linux.h"
#elif OS_IS_WINDOWS
#include "os_windows.h"
#elif OS_IS_WASM
#include "os_wasm.h"
#else
#error Unsupported platform
#endif

// Allocate exactly OS_ALLOC_SIZE bytes of memory
static void *os_alloc(void) {
    // Return cached page (if present)
    if (OS_GLOBAL && OS_GLOBAL->cache) {
        OS_Alloc *alloc = OS_GLOBAL->cache;
        OS_GLOBAL->cache = alloc->next;
        return alloc;
    }

    // Get memory from OS
    return os_alloc_raw(OS_ALLOC_SIZE);
}

// Free entire chain of pages
static void os_free(void *mem) {
    // Add chain to allocation cache
    OS_Alloc *alloc = mem;
    alloc->next = OS_GLOBAL->cache;
    OS_GLOBAL->cache = alloc;
}

static void os_print(char *message) {
    os_write(os_stdout(), (u8 *)message, str_len(message));
}

static OS *os_init(int argc, char **argv) {
    Memory *mem = mem_new();

    OS *os = mem_struct(mem, OS);
    os->argc = argc;
    os->argv = argv;
    OS_GLOBAL = os;

    os->fmt = fmt_file(mem, os_stdout());
    return os;
}
