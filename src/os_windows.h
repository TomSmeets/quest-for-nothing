// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// os.h - api for platform implementations
#pragma once
#include "inc.h"
#include "os.h"

#include <windows.h>
#include <stdio.h>

static void os_assert(bool cond, char *msg) {
    if (cond) return;
    os_print(msg);
    os_print("\n");
    __builtin_debugtrap();
}

static void os_print(char *msg) {
    fputs(msg, stdout);
}

static u64 os_time(void) {
    LARGE_INTEGER big_freq, big_count;
    QueryPerformanceFrequency(&big_freq);
    QueryPerformanceCounter(&big_count);
    i64 freq = big_freq.QuadPart;
    i64 count = big_count.QuadPart;
    assert(freq  >= 1000 * 1000);
    assert(count >= 0);
    u64 time = (u64) count / ((u64) freq / 1000 / 1000);
    return time;
}

// exact sleep until target_time
static void os_sleep_until(u64 target_time) {
    for(;;) {
        u64 time = os_time();
        if(time > target_time) break;

        i64 sleep_time = (i64) (target_time - time)*.5 - 1000;
        if(sleep_time > 0)
            Sleep(sleep_time / 1000);
    }
}

// ==== Memory ====
#define OS_ALLOC_INC (1 * SIZE_MB)

// It is a cache, so we can just clear it on a hot reload
static mem_page *os_page_cache = 0;

static mem_page *os_alloc_page(u64 size) {
    if (size <= OS_ALLOC_INC)
        size = OS_ALLOC_INC;

    // use a previously allocated page
    if (os_page_cache && size == OS_ALLOC_INC) {
        mem_page *p = os_page_cache;
        os_page_cache = p->next;
        p->next = 0;
        assert(p->size == OS_ALLOC_INC);
        return p;
    }

    mem_page *page = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    assert(page);
    page->size = size;
    page->next = 0;
    return page;
}

static void os_free_page(mem_page *page) {
    // put cached pages back into the cache;
    if (page->size == OS_ALLOC_INC) {
        page->next = os_page_cache;
        os_page_cache = page;
        return;
    }

    assert(VirtualFree(page, page->size, MEM_RELEASE));
}
