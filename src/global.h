// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// global.h - all globals in the entire program
#pragma once
#include "os.h"

struct Global {
    mem_page *os_page_cache;
};

static Global *GLOBAL;
