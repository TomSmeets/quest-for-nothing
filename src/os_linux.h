// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// os_linux.h - Platform implementation for linux
#pragma once
#include "fmt.h"
#include "linux_api.h"
#include "os_api.h"
#include "std.h"
#include "str.h"

// Export main, allowing us to dynamically call it
void os_main_dynamic(OS *os) {
    OS_GLOBAL = os;
    os_main(os);
}

static int file_to_fd(File *f) {
    return (int)((u64)f - 1);
}

static File *fd_to_file(int fd) {
    return (File *)((u64)fd + 1);
}

static File *os_stdout(void) {
    return fd_to_file(1);
}

int main(int argc, char **argv) {
    OS *os = os_init(argc, argv);
    for (;;) {
        os_main(os);
        os_sleep(os->sleep_time);
    }
}

// Read
static u64 os_time(void) {
    struct timespec t = {};
    clock_gettime(CLOCK_MONOTONIC, &t);
    return linux_time_to_us(&t);
}

static u64 os_rand(void) {
    u64 seed = 0;
    int fd = open("/dev/urandom", O_RDONLY);
    read(fd, &seed, sizeof(seed));
    close(fd);
    return seed;
}

static void os_write(File *file, u8 *data, u32 len) {
    u32 written = 0;
    while (written < len) {
        ssize_t result = write(file_to_fd(file), data + written, len - written);
        assert(result > 0, "Failed to write");
        written += result;
    }
}

static void os_exit(i32 status) {
    _exit(status);
}

static void os_fail(char *message) {
    write(1, message, str_len(message));
    __builtin_trap();
    _exit(1);
}

static void *os_alloc_raw(u32 size) {
    if (OS_GLOBAL) {
        OS_GLOBAL->stat_alloc_size += size;
        fmt_su(OS_FMT, "os_alloc_raw: total=", OS_GLOBAL->stat_alloc_size / 1024 / 1024, " MB\n");
    }
    int prot = PROT_READ | PROT_WRITE;
    int flags = MAP_PRIVATE | MAP_ANONYMOUS;
    void *alloc = mmap(0, size, prot, flags, -1, 0);
    assert(alloc && alloc != MAP_FAILED, "Failed to allocate memory");
    return alloc;
}

// ==== Desktop ====
static File *os_open(char *path, OS_Open_Type type) {
    int flags = 0;
    if (type == Open_Write) flags |= O_WRONLY | O_CREAT | O_TRUNC;
    if (type == Open_Read) flags |= O_RDONLY;

    int fd = open(path, flags);
    assert(fd >= 0, "Failed to open file");
    return fd_to_file(fd);
}

static void os_close(File *file) {
    assert(file, "File should be valid");
    int ret = close(file_to_fd(file));
    assert(ret == 0, "Failed to close file");
}

static u32 os_read(File *file, u8 *data, u32 len) {
    assert(file, "Input file should be valid");
    ssize_t result = read(file_to_fd(file), data, len);
    assert(result >= 0, "Failed to read");
    return result;
}

static void os_sleep(u64 us) {
    struct timespec time = linux_us_to_time(us);
    nanosleep(&time, 0);
}

static File *os_dlopen(char *path) {
    void *handle = dlopen(path, RTLD_LOCAL | RTLD_NOW);
    if (!handle) os_fail(dlerror());
    return handle;
}

static void *os_dlsym(File *handle, char *name) {
    return dlsym(handle, name);
}
