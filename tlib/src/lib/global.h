// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// global.h - Global state that is preserved between hot-reloads
#pragma once
#include "lib/types.h"

// Forward declare types
// The definitions are defined in each module
typedef struct App App;       // Defined in "main.c"
typedef struct Fmt Fmt;       // Defined in "fmt.h"
typedef struct Rand Rand;     // Defined in "rand.h"
typedef struct Chunk Chunk;   // Defined in "chunk.h"
typedef struct Memory Memory; // Defined in "memory.h"

// NOTE: Only use global in the main thread
typedef struct {
    bool reloaded;

    // Handle to the main application
    // Should be defined in "main.c"
    App *app;

    // Arguments
    u32 argc;
    char **argv;

    // Latest Unique ID counter, starts at 0 and increments forever
    // Used in "id.h"
    u32 id;

    // Formatter for stdout,
    // Should be initialized by the OS
    // Defined in "fmt.h"
    Fmt *fmt;

    // Random number generator
    // Defined in "rand.h"
    Rand *rand;

    // Memory chunck cache. Used internally to track free memory.
    // Used in "chunk.h"
    Chunk *chunk_cache;
    u64 stat_alloc_size;
    u64 stat_cache_size;

    // Global permanent memory
    Memory *mem;

    // Global per frame memory
    Memory *tmp;

    // Timing
    f32 dt;
    u64 time;
    u32 frame_skips;
} Global;

// Global handle, should be initailized by the OS
static Global *G;
