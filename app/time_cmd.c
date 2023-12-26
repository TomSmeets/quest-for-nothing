// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// time_cmd.c - Profile the execution of shell commands
#include "os_generic.h"
#include "fmt.h"

static void print_time(u64 dt_min, u32 i) {
    u64 ms = dt_min / 1000;
    u64 us = (dt_min - ms*1000) / 100;
    os_printf("dt_min = %u.%u ms (i = %u)\n", ms, us, i);
}

void *main_init(int argc, char **argv) {
    assert(argc == 2);
    char *cmd = argv[1];
    os_printf("Timing: '%s'\n", cmd);

    u64 dt_min = U64_MAX;
    u32 i = 0;
    for(; i < 1024*2; ++i) {
        u64 t0 = os_time();
        i32 code = os_command(cmd);
        if(code != 0) os_exit(code);
        u64 t1 = os_time();
        u64 dt = t1 - t0;
        if(dt < dt_min) {
            dt_min = dt;
            print_time(dt_min, i);
        }
    }
    print_time(dt_min, i);
    return 0;
}

void main_update(void *handle) { os_exit(0); }
