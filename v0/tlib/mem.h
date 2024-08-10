// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// mem.h - a simple stack based memory allocator
#pragma once
#include "tlib/os.h"

#define mem_struct(m, t) ((t *)mem_push_ex(m, sizeof(t), _Alignof(t), 1))
#define mem_array(m, t, n) ((t *)mem_push_ex(m, sizeof(t) * (n), _Alignof(t), 1))
#define mem_array_uninit(m, t, n) ((t *)mem_push_ex(m, sizeof(t) * (n), _Alignof(t), 0))

// Allocates smaller chucks of memory from bigger memory pages
typedef Memory mem;
struct Memory {
    Memory_Page *used_page;
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
static void *mem_push_ex(Memory *m, u64 size, u64 align, bool clear) {
    // determine the allocated region boundary
    u8 *alloc_start = mem_align(m->start, align);
    u8 *alloc_end = alloc_start + size;

    if (alloc_end > m->end) {
        // if it does not fit, allocate a new page with the minimum size
        Memory_Page *p = os_alloc_page(sizeof(Memory_Page) + size);
        assert(p && p->size >= sizeof(Memory_Page) + size);
        p->next = m->used_page;
        m->used_page = p;

        m->start = (u8 *)p + sizeof(Memory_Page);
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

static void *mem_push(Memory *m, u64 size) {
    return mem_push_ex(m, size, 8, 0);
}

// free all used pages
static void mem_clear(Memory *m) {
    Memory_Page *used_page = m->used_page;
    m->start = 0;
    m->end = 0;
    m->used_page = 0;

    for (;;) {
        Memory_Page *p = used_page;
        if (!p)
            break;
        used_page = p->next;
        os_free_page(p);
    }
}

// Create a new memory arena inside itself.
// The pointer is invalid after calling mem_free/mem_clear
static Memory *mem_new(void) {
    Memory m = {};
    Memory *m2 = mem_struct(&m, Memory);
    *m2 = m;
    return m2;
}

static void mem_free(Memory *m) {
    mem_clear(m);
}
