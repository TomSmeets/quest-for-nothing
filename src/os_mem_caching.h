// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// os_mem_caching.h - Wrapper to enable memory page caching
#pragma once
#include "os.h"
#include "global.h"

// The (maximum) page size we will cache
#define OS_ALLOC_INC (1 * SIZE_MB)

// Implement these uncached versions
static mem_page *os_alloc_page_uncached(u64 size);
static void os_free_page_uncached(mem_page *page);

// Allocate a memory page of at least 'size' bytes, or some page increment
static mem_page *os_alloc_page(u64 size) {
    if (size < OS_ALLOC_INC)
        size = OS_ALLOC_INC;

    // Try the cache
    if(GLOBAL->os_page_cache && size == OS_ALLOC_INC) {
        mem_page *page = GLOBAL->os_page_cache;
        GLOBAL->os_page_cache = page->next;
        page->next = 0;
        assert(page->size == OS_ALLOC_INC);
        return page;
    }

    // Otherwise, Allocate the page
    return os_alloc_page_uncached(size);
}

static void os_free_page(mem_page *page) {
    // Return the page to the cache
    if (page->size == OS_ALLOC_INC) {
        page->next = GLOBAL->os_page_cache;
        GLOBAL->os_page_cache = page;
        return;
    }

    // Otherwsie deallocate the page
    return os_free_page_uncached(page);
}
