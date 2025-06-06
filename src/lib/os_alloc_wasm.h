// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// os_alloc_wasm.h - Minimal OS functinality implemented for WASM
#pragma once
#include "lib/os_api.h"
#include "lib/str.h"

#if 0
#include "lib/os_alloc_wasm.js"
#endif

// Webassembly can only grow a linear heap
// This is fine as long as we only allocate
static void *os_alloc(u32 size) {
    // Calculate the number of pages to allocate (by rounding up)
    i32 pages = (size + WASM_PAGE_SIZE - 1) / WASM_PAGE_SIZE;
    u32 addr = (u64)__builtin_wasm_memory_grow(0, pages) * WASM_PAGE_SIZE;
    return (void *)addr;
}

WASM_IMPORT(wasm_fail) void wasm_fail(char *message, u32 len);
static void os_fail(char *message) {
    wasm_fail(message, str_len(message));
}
