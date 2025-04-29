// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// global.h - Global state that is preserved between hot-reloads
#pragma once
#include "types.h"

typedef struct App App;
typedef struct Fmt Fmt;
typedef struct Rand Rand;
typedef struct OS OS;
typedef struct Chunk Chunk;

typedef struct {
    bool reloaded;

    u32 id;             // id.h
    App *app;           // app.h
    Fmt *fmt;           // fmt.h
    Rand *rand;         // rand.h
    OS *os;             // os.h
    Chunk *chunk_cache; // chunk.h
} Global;

static Global *G;
