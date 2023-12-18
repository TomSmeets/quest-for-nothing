// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// mem.h - a simple stack based memory allocator
#pragma once
#include "inc.h"
#include "os.h"

#define mem_struct(m, t)   ((t *)mem_push_ex(m, sizeof(t), _Alignof(t), 1))
#define mem_array(m, t, n) ((t *)mem_push_ex(m, sizeof(t)*(n), _Alignof(t), 1))

// Allocates smaller chucks of memory from bigger memory pages
struct mem {
    mem_page *used_page;
    u8 *start;
    u8 *end;
};

// Align a pointer upwards
static void *mem_align(void *p, u64 align) {
    u64 mask = align - 1;

    // make sure align is a power of two
    assert((mask & align) == 0);

    return (void *)(((u64)p + mask) & ~mask);
}

// allocate a region of a given size
// Alignment. Always 8? == simple
static void *mem_push_ex(mem *m, u64 size, u64 align, bool clear) {
    // determine the allocated region boundary
    u8 *alloc_start = mem_align(m->start, align);
    u8 *alloc_end = alloc_start + size;

    if (alloc_end > m->end) {
        // if it does not fit, allocate a new page with the minimum size
        mem_page *p = os_alloc_page(sizeof(mem_page) + size);
        p->next = m->used_page;
        m->used_page = p;

        m->start = (u8 *)p + sizeof(mem_page);
        m->end = (u8 *)p + p->size;

        alloc_start = mem_align(m->start, align);
        alloc_end = alloc_start + size;
        assert(alloc_end <= m->end);
    }

    m->start = alloc_end;

    if (clear) {
        std_memzero(alloc_start, size);
    }

    return alloc_start;
}

static void *mem_push(mem *m, u64 size) {
    return mem_push_ex(m, size, 8, 0);
}

// free all used pages
static void mem_clear(mem *m) {
    for (;;) {
        mem_page *p = m->used_page;
        if (!p) break;
        m->used_page = p->next;

        // don't use p after this
        os_free_page(p);
    }

    m->start = 0;
    m->end = 0;
}
