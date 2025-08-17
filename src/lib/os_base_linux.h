// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// os_base_linux.h - Base OS implementation for Linux
#pragma once
#include "lib/os_alloc.h"
#include "lib/os_api.h"
#include "lib/std.h"
#include "lib/str.h"
#include "lib/types.h"

static u64 os_time(void) {
    struct linux_timespec t = {};
    linux_clock_gettime(CLOCK_MONOTONIC, &t);
    return linux_time_to_us(&t);
}

static u64 os_rand(void) {
    u64 seed = 0;
    i64 ret = linux_getrandom(&seed, sizeof(seed), 0);
    assert(ret == sizeof(seed), "linux getrandom failed");
    return seed;
}

static void os_write(File *file, u8 *data, u32 len) {
    u32 written = 0;
    while (written < len) {
        i64 result = linux_write(fd_from_file(file), data + written, len - written);
        assert(result > 0, "Failed to write");
        written += result;
    }
}

static void os_exit(i32 status) {
    linux_exit_group(status);
}
