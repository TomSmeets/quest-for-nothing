// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// os_linux.h - Platform implementation for linux
#pragma once
#include "linux_api.h"
#include "os_api.h"
#include "std.h"
#include "str.h"

static void os_exit(i32 status) {
    _exit(status);
}

static void os_write(u32 fd, u8 *data, u32 len) {
    ssize_t result = write(fd, data, len);
    assert(result >= 0, "Failed to write");
    assert(result == len, "Failed to write all");
}

static u32 os_read(u32 fd, u8 *data, u32 len) {
    ssize_t result = read(fd, data, len);
    assert(result >= 0, "Failed to read");
    return result;
}

static void os_fail(char *message) {
    write(1, message, str_len(message));
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
        os_sleep(os.sleep_time);
    }
}

static u32 os_open(char *path, OS_Open_Type type) {
    int flags = 0;
    if(type == Open_Write) flags |= O_WRONLY | O_CREAT | O_TRUNC;
    if(type == Open_Read)  flags |= O_RDONLY;

    int fd = open(path, flags);
    assert(fd >= 0, "Failed to open file");
    return fd;
}

static void os_close(u32 fd) {
    int ret = close(fd);
    assert(ret == 0, "Failed to close file");
}
