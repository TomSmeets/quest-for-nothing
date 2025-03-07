// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// chunk.h - Chunk allocator
#pragma once

// chunck_cache is a global variable
#include "global.h"

// Memory can be requested from the operating system at any time.
#include "os_alloc.h"

// This memory is never given back to the operating system.
// We are managing it ourselves.

// Memory is allocaed from the operating system in big fixed-size Chunks.
// - Every chunk is the same and can be reused anywehre
// - Every chunk should be big enough to hold the biggest structure.
// - Every chunck is eitehr 'free' or 'used'.

// Every `Chunk` has a fixed size of exactly 1 MB
#define CHUNK_SIZE (1 * 1024 * 1024)

typedef struct Chunk Chunk;

struct Chunk {
    // Each chunk has a small header that can point to the next chunk, creating a linked-list.
    // The next pointer is embedded in the chunck memory.
    Chunk *next;

    // The rest of the chunck is memory used or free memory
    u8 data[];
};

// Allocate a new chunck
static Chunk *chunck_alloc(void) {
    // Check if there are free chunks
    if (G->chunk_cache) {
        // Remove the free chunk from the cache
        Chunk *chunk = G->chunk_cache;
        G->chunk_cache = chunk->next;
        chunk->next = 0;

        // Give it to the caller
        return chunk;
    }

    // The cache is empty, ask the os for a new chunk.
    return os_alloc(CHUNK_SIZE);
}

// Free all used chunks in this list and add the to the cache
static void chunck_free(Chunk *first) {
    // Find last element in the list
    Chunk *last = first;
    while (last->next) last = last->next;

    // Prepend the chuncks to the cache
    last->next = G->chunk_cache;
    G->chunk_cache = first;
}
