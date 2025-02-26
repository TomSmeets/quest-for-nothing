// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// global.h - Global state that is preserved between hot-reloads
#pragma once
#include "types.h"

typedef struct App App;

typedef struct {
    bool reloaded;
    u32 id;
    App *app;
    struct Fmt *fmt;
    struct Rand *rand;
    struct OS *os;
    struct Memory_Chunk *mem_cache;
} Global;

static Global _GLOBAL_INSTANCE;
static Global *G = &_GLOBAL_INSTANCE;
