// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// os_alloc_windows.h - Minimal OS functinality implemented for Windows
#pragma once
#include "lib/os_api.h"

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
