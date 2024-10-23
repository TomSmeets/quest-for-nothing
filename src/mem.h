// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// mem.h - Simple memory allocator
#pragma once
#include "std.h"

// Maximum allocation size
// The exact size of a memory "Chunk"
#define MEMORY_CHUNK_SIZE (1 * 1024 * 1024)

#define mem_struct(mem, type) ((type *)mem_push_zero((mem), sizeof(type)))
#define mem_array_uninit(mem, type, count) ((type *)mem_push_uninit((mem), sizeof(type) * (count)))

// A big chunk of memory of exactly MEMORY_CHUNK_SIZE bytes
typedef struct Memory_Chunk Memory_Chunk;
struct Memory_Chunk {
    Memory_Chunk *next;
};

// A push-based memory allocator.
// Smaller allocations are added to a big Memory_Chunk until it is full.
// Once full, a new chunk is allocated.
// All allocations are freed simultaneously with mem_free
typedef struct {
    Memory_Chunk *start;
    u32 used;
    u32 size;
} Memory;

// Align the next allocation to 16 bytes
static void _mem_align(Memory *mem) {
    mem->used = (mem->used + 15) & ~15;
}

// Allocate a chunk of exactly MEMORY_CHUNK_SIZE bytes
static void *mem_alloc_chunk(void) {
    // Return cached chunk (if present)
    if (OS_GLOBAL && OS_GLOBAL->memory_cache) {
        Memory_Chunk *alloc = OS_GLOBAL->memory_cache;
        OS_GLOBAL->memory_cache = alloc->next;
        return alloc;
    }

    // Get new memory from OS
    return os_alloc_raw(MEMORY_CHUNK_SIZE);
}

// Free a list of memory chunks allocated by mem_alloc_chunk
static void mem_free_chunk_list(Memory_Chunk *chunk) {
    // Find last element in the list
    Memory_Chunk *last = chunk;
    while(last->next) last = last->next;

    // Add it to the free chunk list
    last->next = OS_GLOBAL->memory_cache;
    OS_GLOBAL->memory_cache = last;
}

// Free memory allocated by mem_alloc_chunk
static void mem_free_chunk(void *mem) {
    Memory_Chunk *chunk = mem;
    // Add it to the free chunk list
    chunk->next = OS_GLOBAL->memory_cache;
    OS_GLOBAL->memory_cache = chunk;
}

// Allocate 'size' bytes of uninitialized memory
static void *mem_push_uninit(Memory *mem, u32 size) {
    _mem_align(mem);

    // Out of memory
    if (mem->used + size > mem->size) {
        Memory_Chunk *alloc = mem_alloc_chunk();
        alloc->next = mem->start;

        mem->start = alloc;
        mem->size = MEMORY_CHUNK_SIZE;
        mem->used = sizeof(Memory_Chunk);
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
