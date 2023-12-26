// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// global.h - Keep track of all globals here
#pragma once
#include "os.h"

struct Global {
    mem_page *os_page_cache;
};

static Global *GLOBAL;

static void global_set(Global *global) { GLOBAL = global; }
