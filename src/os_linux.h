// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// os.h - Platform implementation for linux
#pragma once
#include "linux_api.h"
#include "os_api.h"
#include "std.h"
#include "str.h"

static void os_exit(i32 status) {
    _exit(status);
}

static void os_write(u32 fd, u8 *msg, u32 len) {
    ssize_t result = write(fd, msg, len);
    assert(result == len, "Failed to write");
}

static void os_fail(char *message) {
    write(2, message, str_len(message));
    _exit(1);
}

static void *os_alloc_raw(u32 size) {
    os_print("OS_ALLOC\n");
    int prot = PROT_READ | PROT_WRITE;
    int flags = MAP_PRIVATE | MAP_ANONYMOUS;
    void *alloc = mmap(0, size, prot, flags, -1, 0);
    assert(alloc && alloc != MAP_FAILED, "Failed to allocate memory");
    return alloc;
}

static u64 os_time(void) {
    struct timespec t = {};
    clock_gettime(CLOCK_MONOTONIC, &t);
    return linux_time_to_us(&t);
}

static void os_sleep(u64 us) {
    struct timespec time = linux_us_to_time(us);
    nanosleep(&time, 0);
}

static void *os_load_sdl2(char *name) {
    OS *os = OS_GLOBAL;

    if (!os->sdl2_handle) {
        os->sdl2_handle = dlopen("libSDL2.so", RTLD_LOCAL | RTLD_NOW);
    }

    return dlsym(os->sdl2_handle, name);
}

static u64 os_rand(void) {
    u64 seed = 0;
    int fd = open("/dev/urandom", O_RDONLY);
    read(fd, &seed, sizeof(seed));
    close(fd);
    return seed;
}

// Export main, allowing us to dynamically call it
void os_main_dynamic(OS *os) {
    OS_GLOBAL = os;
    os_main(os);
}

int main(int argc, const char **argv) {
    OS os = {};
    os.argc = argc;
    os.argv = (char **)argv;
    for (;;) {
        os_main_dynamic(&os);
    }
}
