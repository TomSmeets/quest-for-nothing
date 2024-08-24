// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// id.h - Generate unique ids for fast comparisons
#pragma once
#include "os.h"

// Return a application global unique id.
// Id is always > 0
static u32 id_next(void) {
    return ++OS_GLOBAL->uid;
}
