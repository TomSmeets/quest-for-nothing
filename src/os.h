// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// os.h - Platform api
#pragma once
#include "os_api.h"
#include "types.h"

#if __linux__
#define OS_IS_LINUX 1
#include "os_linux.h"
#else
#define OS_IS_WINDOWS 1
#include "os_windows.h"
#endif

// Allocate memory
static OS_Alloc *os_alloc(void) {
    // Return cached page (if present)
    if (OS_GLOBAL->cache) {
        OS_Alloc *alloc = OS_GLOBAL->cache;
        OS_GLOBAL->cache = alloc->next;
        return alloc;
    }

    // Get memory from OS
    return os_alloc_raw(OS_ALLOC_SIZE);
}

// Free entire chain of pages
static void os_free(OS_Alloc *ptr) {
    // Find last page
    OS_Alloc *last = ptr;
    while (last->next)
        last = last->next;

    // Add chain to allocation cache
    last->next = OS_GLOBAL->cache;
    OS_GLOBAL->cache = ptr;
}

static void os_print(char *msg) {
    os_write((u8 *)msg, str_len(msg));
}
