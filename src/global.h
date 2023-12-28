// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// global.h - Keep track of all globals here
#pragma once
#include "os.h"

struct Global {
    bool did_reload;
    mem_page *os_page_cache;
};

static Global *GLOBAL;

static void global_set(Global *global) { global->did_reload = global != GLOBAL; GLOBAL = global; }
