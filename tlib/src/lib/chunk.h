// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// chunk.h - Memory allocator in big chunks
#pragma once

// chunk_cache is a global variable
#include "lib/global.h"

// Memory can be requested from the operating system at any time.
#include "lib/os_alloc.h"

// This memory is never given back to the operating system.
// We are managing it ourselves.

// Memory is allocaed from the operating system in big fixed-size Chunks.
// - Every chunk is the same and can be reused anywehre
// - Every chunk should be big enough to hold the biggest structure.
// - Every chunk is eitehr 'free' or 'used'.

// Every `Chunk` has a fixed size of exactly 1 MB
#define CHUNK_SIZE (2 * 1024 * 1024)
#define CHUNK_HEADER_SIZE (sizeof(void *))

typedef struct Chunk Chunk;

struct Chunk {
    // Each chunk has a small header that can point to the next chunk, creating a linked-list.
    // The next pointer is embedded in the chunk memory.
    Chunk *next;

    // The rest of the chunk is memory, used or free
    // ...
};

// Allocate a new chunk
static Chunk *chunk_alloc(void) {
    // Check if there are free chunks
    if (G->chunk_cache) {
        // Remove the free chunk from the cache
        Chunk *chunk = G->chunk_cache;
        G->chunk_cache = chunk->next;
        chunk->next = 0;
        G->stat_cache_size -= CHUNK_SIZE;

        // Give it to the caller
        return chunk;
    }

    // The cache is empty, ask the os for a new chunk.
    return os_alloc(CHUNK_SIZE);
}

// Free all used chunks in this list and add the to the cache
static void chunk_free(Chunk *first) {
    Chunk *chunk = first;
    while (chunk) {
        Chunk *next = chunk->next;
        LIST_PUSH(G->chunk_cache, chunk, next);
        G->stat_cache_size += CHUNK_SIZE;
        chunk = next;
    }
}
