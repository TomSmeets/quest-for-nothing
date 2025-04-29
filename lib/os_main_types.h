// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// os_main_types.h - Type definitions for os_main
#pragma once
#include "fmt.h"
#include "global.h"
#include "os_desktop.h"
#include "rand.h"
#include "types.h"
#include "types.h"

typedef struct OS OS;
struct OS {
    // Command line args
    u32 argc;
    char **argv;

    // Time to sleep until the next time
    // os_main is called.
    u64 sleep_time;
};

// Main callback, implement this method for your application
// os_main is called in a infinite loop, until os_exit is called
// The command line arguments and other members of the `OS` struct
// can be accessed with `G->os`
static void os_main(void);
