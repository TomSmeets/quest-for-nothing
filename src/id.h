#pragma once

#include "os.h"

// Return a application global uniqe id.
// Id is always > 0
static u64 id_next(void) {
    return ++OS_GLOBAL->uid;
}
