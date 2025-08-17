// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// id.h - Generate unique ids for fast comparisons
#pragma once
#include "lib/global.h"

// Comparing images is expensive because they contain a lot of data. Using an
// unique id allows for quick equality comparison. Just increment the id when
// an image is changed.

// Return a application global unique id. Id is always > 0
static u32 id_next(void) {
    return ++G->id;
}
