// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// mem.h - Simple memory allocator
#pragma once
#include "os.h"
#include "std.h"
#include "types.h"

#define mem_struct(mem, type) ((type *)mem_push_zero((mem), sizeof(type)))
#define mem_array_uninit(mem, type, count) ((type *)mem_push_uninit((mem), sizeof(type) * (count)))

typedef struct {
    OS_Alloc *start;
    u32 used;
    u32 size;
} Memory;

static void *mem_push_uninit(Memory *mem, u32 size) {
    // Align to 8 bytes (assuming start is also aligned)
    mem->used = (mem->used + 7) & ~7;

    // Out of memory
    if (mem->used + size > mem->size) {
        OS_Alloc *alloc = os_alloc();
        alloc->next = mem->start;
        mem->start = alloc;
        mem->size = OS_ALLOC_SIZE;
        mem->used = sizeof(OS_Alloc);

        // Align again
        mem->used = (mem->used + 7) & ~7;

        if (mem->used + size > mem->size) {
            return 0;
        }
    }

    // Allocate memory
    void *ptr = (void *)mem->start + mem->used;
    mem->used += size;
    return ptr;
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

static void mem_free(Memory *mem) {
    os_free(mem->start);
}
