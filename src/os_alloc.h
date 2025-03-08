// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// os_alloc.h - os_alloc implmentation
#pragma once
#include "os_api.h"

// Allocate memory from the system
static void *os_alloc(u32 size);

// Kill the current process with an error message
static void os_fail(char *message);

#define assert(cond, msg)                                                                                                                            \
    if (!(cond)) os_fail("Assertion failed: " #cond " " msg "\n")

#if OS_IS_LINUX
// On Linux, we can use the 'mmap' system-call.
// mmap can also map files to memory, which is not used in this case.
static void *os_alloc(u32 size) {
    void *ret = linux_mmap(
        // Let the system choose a starting address for us
        0,
        // Allocation size
        size,
        // Memory should be read and writable
        PROT_READ | PROT_WRITE,
        // Allocation is not backed by a file
        MAP_PRIVATE | MAP_ANONYMOUS,
        // File descriptor and offset are not used
        -1, 0
    );
    if(ret == MAP_FAILED) return 0;
    return ret;
}

static void os_fail(char *message) {
    linux_write(1, message, str_len(message));
    linux_exit_group(1);
}

#endif

#if OS_IS_WINDOWS
// On Windows has a simmilar call called 'VirtualAlloc'
static void *os_alloc(u32 size) {
    return VirtualAlloc(
        // Let the system choose a starting address for us
        0,
        // Allocation size
        size,
        // Reserve the address range and commit the memory in that range
        MEM_COMMIT | MEM_RESERVE,
        // Memory should be read and writable
        PAGE_READWRITE
    );
}

static void os_fail(char *message) {
    MessageBox(NULL, message, "Error", MB_ICONERROR | MB_OK);
}
#endif


#if OS_IS_WASM
// Webassembly can only grow a linear heap
// This is fine as long as we only allocate
static void *os_alloc(u32 size) {
    // Calculate the number of pages to allocate (by rounding up)
    i32 pages = (size + WASM_PAGE_SIZE - 1) / WASM_PAGE_SIZE;
    u32 addr = (u64)__builtin_wasm_memory_grow(0, pages) * WASM_PAGE_SIZE;
    return (void *)addr;
}

WASM_IMPORT(wasm_fail) void wasm_fail(char *);
static void os_fail(char *message) {
    wasm_fail(message);
}
#endif
