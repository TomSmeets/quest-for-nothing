// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// global.h - Global state that is preserved between hot-reloads
#pragma once
#include "types.h"

// Forward declare types
// The definitions are defined in each module
typedef struct App App;     // Defined in "main.c"
typedef struct OS OS;       // Defined in "os_main.h"
typedef struct Fmt Fmt;     // Defined in "fmt.h"
typedef struct Rand Rand;   // Defined in "rand.h"
typedef struct Chunk Chunk; // Defined in "chunk.h"

typedef struct {
    bool reloaded;

    // Handle to the main application
    // Should be defined in "main.c"
    App *app;

    // Handle to operating system specific state
    // Is only used by OS spesific code
    // Defined in "os_main.h"
    OS *os;

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
} Global;

// Global handle, should be initailized by the OS
static Global *G;
