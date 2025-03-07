// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// mem.h - Simple memory allocator
#pragma once
#include "chunk.h"

typedef struct Memory Memory;

// A stack allocator for variable size allocations.
// Each allocation should be smaller than the chunk size.
struct Memory {
    // A list of used chunks.
    // The first entry is still being used for new allocations.
    Chunk *chunk;

    // Bytes used in the current chunck
    // This includes the chunk header
    u32 used;

    // Total size of the first chunk (always 1 MB)
    u32 size;
};

// Align the next allocation to 16 bytes
static void _mem_align(Memory *mem) {
    mem->used = (mem->used + 15) & ~15;
}

#define mem_struct(mem, type) ((type *)mem_push_zero((mem), sizeof(type)))
#define mem_array_uninit(mem, type, count) ((type *)mem_push_uninit((mem), sizeof(type) * (count)))

// Allocate 'size' bytes of uninitialized memory
static void *mem_push_uninit(Memory *mem, u32 size) {
    _mem_align(mem);

    // Out of memory
    if (mem->used + size > mem->size) {
        Chunk *alloc = mem_alloc_chunk();
        alloc->next = mem->start;

        mem->start = alloc;
        mem->size = CHUNK_SIZE;
        mem->used = sizeof(Chunk);
        _mem_align(mem);

        assert(mem->used + size <= mem->size, "This allocation does not fit!");
    }

    // Allocate memory
    void *ptr = (void *)mem->start + mem->used;
    mem->used += size;
    return ptr;
}

// Allocate exactly 'size' bytes of memory and clear it to zero
static void *mem_push_zero(Memory *mem, u32 size) {
    void *ptr = mem_push_uninit(mem, size);
    std_memzero(ptr, size);
    return ptr;
}

// Number of bytes remaining in the current memory chunk
// A bigger allocation would create a new chunk
static u32 mem_remaining(Memory *mem) {
    return mem->size - mem->used;
}

// Allocate a new memory allocator.
static Memory *mem_new(void) {
    Memory mem_tmp = {};
    Memory *mem = mem_struct(&mem_tmp, Memory);
    *mem = mem_tmp;
    return mem;
}

// Free this memory allocator and all it's allocations
static void mem_free(Memory *mem) {
    mem_free_chunk_list(mem->start);
}
