#pragma once
#include "os_alloc.h"
#include "std.h"
#include "str.h"
#include "types.h"

#include "linux_api.h"

static u64 os_time(void) {
    struct linux_timespec t = {};
    linux_clock_gettime(CLOCK_MONOTONIC, &t);
    return linux_time_to_us(&t);
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

