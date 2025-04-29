// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// mem.h - Simple memory allocator
#pragma once
#include "chunk.h"
#include "types.h"

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
static void u32_align(u32 *addr, u32 bytes) {
    u32 mask = bytes - 1;
    *addr = (*addr + mask) & ~mask;
}

// Allocate 'size' bytes of uninitialized memory
static void *mem_push_uninit(Memory *mem, u32 size) {
    // Primitives should be aligned to their own size.
    //   int8 -> no alignment needed
    //   int32 -> 4 byte alignment
    //   int64 -> 8 byte alignment
    //   float -> 4 byte alignment
    //
    // SIMD registers should also be aligned
    //   float4 -> 16 byte alignment
    //
    // Assuming 16 byte alignment for all allocations
    u32 align = 16;
    u32_align(&mem->used, align);

    // Check if the allocation will fit
    if (mem->used + size > mem->size) {
        // The allocation odes not fit in the current chunk.
        // We need to allocat a new chunk.
        Chunk *chunk = chunk_alloc();

        // This chunk is the new 'current'
        // Fhe previous chunk is now full
        chunk->next = mem->chunk;
        mem->chunk = chunk;

        // Chunks are always the same size
        mem->size = CHUNK_SIZE;
        mem->used = CHUNK_HEADER_SIZE;

        // Redo the alignment
        u32_align(&mem->used, align);

        // This allocation does not fit
        if (mem->used + size > mem->size) {
            return 0;
        }
    }

    // Allocate the memory from the current
    void *ptr = (void *)mem->chunk + mem->used;
    mem->used += size;
    return ptr;
}

// Allocate exactly 'size' bytes of zero initialized memory
static void *mem_push_zero(Memory *mem, u32 size) {
    void *ptr = mem_push_uninit(mem, size);
    std_memzero(ptr, size);
    return ptr;
}

// Allocate a zero-initialised type
#define mem_struct(mem, type) ((type *)mem_push_zero((mem), sizeof(type)))

// Allocate an un-initialised array of types
#define mem_array_uninit(mem, type, count) ((type *)mem_push_uninit((mem), sizeof(type) * (count)))

// Allocate a zero-initialised array of types
#define mem_array_zero(mem, type, count) ((type *)mem_push_zero((mem), sizeof(type) * (count)))

// Create a memory allocator.
// The returned structure is allocaed in the first chunk.
static Memory *mem_new(void) {
    // Temporarly store the arena on the stack
    Memory mem_stack = {};

    // Move the arena into its own first chunk
    Memory *mem_heap = mem_struct(&mem_stack, Memory);
    *mem_heap = mem_stack;

    return mem_heap;
}

// Free this memory allocator and all it's allocations
static void mem_free(Memory *mem) {
    // Free all chunks in this memory arena
    chunk_free(mem->chunk);
}
