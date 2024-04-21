// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// global.h - Keep track of all globals here
#pragma once
#include "tlib/os.h"

struct Global {
    bool did_reload;
    Memory_Page *os_page_cache;
};

static Global *GLOBAL;

static void global_set(Global *global) {
    global->did_reload = global != GLOBAL;
    GLOBAL = global;
}
