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
    u32 id;
    App *app;
    Fmt *fmt;
    Rand *rand;
    OS *os;

    Chunk *chunk_cache;
} Global;

static Global _GLOBAL_INSTANCE;
static Global *G = &_GLOBAL_INSTANCE;
