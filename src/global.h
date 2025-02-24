// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// global.h - Global state
#pragma once
#include "types.h"

typedef struct {
    bool reload;
    u32 id;
    void *app;
    void *fmt;
    void *rand;
    void *os;
    void *mem;
} Global;

static Global *G;
