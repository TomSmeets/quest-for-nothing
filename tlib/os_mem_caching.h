// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// os_mem_caching.h - Wrapper to enable memory page caching
#pragma once
#include "tlib/global.h"
#include "tlib/os.h"

// The (maximum) page size we will cache
#define OS_ALLOC_INC (1 * SIZE_MB)

// Implement these uncached versions
static Memory_Page *os_alloc_page_uncached(u64 size);
static void os_free_page_uncached(Memory_Page *page);

// Allocate a memory page of at least 'size' bytes, or some page increment
static Memory_Page *os_alloc_page(u64 size) {
    if (size < OS_ALLOC_INC)
        size = OS_ALLOC_INC;

    // Try the cache
    if (GLOBAL && GLOBAL->os_page_cache && size == OS_ALLOC_INC) {
        Memory_Page *page = GLOBAL->os_page_cache;
        GLOBAL->os_page_cache = page->next;
        page->next = 0;
        assert(page->size == OS_ALLOC_INC);
        return page;
    }

    // Otherwise, Allocate the page
    return os_alloc_page_uncached(size);
}

static void os_free_page(Memory_Page *page) {
    // Return the page to the cache
    if (GLOBAL && page->size == OS_ALLOC_INC) {
        page->next = GLOBAL->os_page_cache;
        GLOBAL->os_page_cache = page;
        return;
    }

    // Otherwsie deallocate the page
    return os_free_page_uncached(page);
}
