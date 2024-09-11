// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// mem.h - Simple memory allocator
#pragma once
#include "os_api.h"
#include "std.h"
#include "types.h"

#define mem_struct(mem, type) ((type *)mem_push_zero((mem), sizeof(type)))
#define mem_array_uninit(mem, type, count) ((type *)mem_push_uninit((mem), sizeof(type) * (count)))

typedef struct {
    OS_Alloc *start;
    u32 used;
    u32 size;
} Memory;

// Align to 16 bytes
static void mem_align(Memory *mem) {
    mem->used = (mem->used + 15) & ~15;
}

static void *mem_push_uninit(Memory *mem, u32 size) {
    mem_align(mem);

    // Out of memory
    if (mem->used + size > mem->size) {
        OS_Alloc *alloc = os_alloc();
        alloc->next = mem->start;
        mem->start = alloc;
        mem->size = OS_ALLOC_SIZE;

        mem->used = sizeof(OS_Alloc);
        mem_align(mem);

        assert(mem->used + size <= mem->size, "This allocation does not fit!");
    }

    // Allocate memory
    void *ptr = (void *)mem->start + mem->used;
    mem->used += size;
    return ptr;
}

// Number of bytes remaining
static u32 mem_remaining(Memory *mem) {
    return mem->size - mem->used;
}

static void *mem_push_zero(Memory *mem, u32 size) {
    void *ptr = mem_push_uninit(mem, size);
    std_memzero(ptr, size);
    return ptr;
}

static Memory *mem_new(void) {
    Memory mem_tmp = {};
    Memory *mem = mem_struct(&mem_tmp, Memory);
    *mem = mem_tmp;
    return mem;
}

static void mem_free_alloc(OS_Alloc *alloc) {
    if(alloc->next) mem_free_alloc(alloc->next);
    os_free(alloc);
}

static void mem_free(Memory *mem) {
    mem_free_alloc(mem->start);
}
