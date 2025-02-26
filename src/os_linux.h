// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// os_linux.h - Platform implementation for linux
#pragma once
#include "fmt.h"
#include "global.h"
#include "linux_api.h"
#include "os.h"
#include "rand.h"
#include "str.h"

static u64 linux_time_to_us(struct linux_timespec *t) {
    return t->sec * 1000 * 1000 + t->nsec / 1000;
}

static struct linux_timespec linux_us_to_time(u64 time) {
    u64 sec = time / (1000 * 1000);
    u64 nsec = (time - sec * 1000 * 1000) * 1000;

    struct linux_timespec ts;
    ts.sec = sec;
    ts.nsec = nsec;
    return ts;
}

static i32 file_to_fd(File *f) {
    return (i32)((u64)f - 1);
}

static File *fd_to_file(i32 fd) {
    return (File *)((u64)fd + 1);
}

// Export main, allowing us to dynamically call it
void os_main_dynamic(Global *global_instance) {
    G = global_instance;
    os_main();
}

int main(int argc, char **argv) {
    OS os = {};
    os.argc = argc;
    os.argv = argv;
    G->os = &os;

    Fmt fmt = {};
    fmt.out = fd_to_file(1);
    G->fmt = &fmt;

    Rand rand = rand_new(os_rand());
    G->rand = &rand;

    for (;;) {
        os.sleep_time = 1000 * 1000;
        os_main();
        os_sleep(os.sleep_time);
    }
}

// Read
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
        i64 result = linux_write(file_to_fd(file), data + written, len - written);
        assert(result > 0, "Failed to write");
        written += result;
    }
}

static void os_exit(i32 status) {
    linux_exit_group(status);
}

static void os_fail(char *message) {
    linux_write(1, message, str_len(message));
    __builtin_trap();
    linux_exit_group(1);
}

static void *os_alloc_raw(u32 size) {
    i32 prot = PROT_READ | PROT_WRITE;
    i32 flags = MAP_PRIVATE | MAP_ANONYMOUS;
    void *alloc = linux_mmap(0, size, prot, flags, -1, 0);
    assert(alloc && alloc != MAP_FAILED, "Failed to allocate memory");
    return alloc;
}

// ==== Desktop ====
static File *os_open(char *path, OS_Open_Type type) {
    i32 fd = -1;
    if (type == Open_Write) {
        fd = linux_open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    } else if (type == Open_Read) {
        fd = linux_open(path, O_RDONLY, 0);
    }

    // Failed to open file
    if (fd <= 0) return 0;

    return fd_to_file(fd);
}

static void os_close(File *file) {
    assert(file, "File should be valid");
    i32 ret = linux_close(file_to_fd(file));
    assert(ret == 0, "Failed to close file");
}

static u32 os_read(File *file, u8 *data, u32 len) {
    assert(file, "Input file should be valid");
    i64 result = linux_read(file_to_fd(file), data, len);
    assert(result >= 0, "Failed to read");
    return result;
}

static void os_sleep(u64 us) {
    struct linux_timespec time = linux_us_to_time(us);
    linux_nanosleep(&time, 0);
}

static File *os_dlopen(char *path) {
    return dlopen(path, RTLD_LOCAL | RTLD_NOW);
}

static void *os_dlsym(File *handle, char *name) {
    return dlsym(handle, name);
}

static char *os_dlerror(void) {
    return dlerror();
}

static bool os_system(char *cmd) {
    fmt_ss(G->fmt, "> ", cmd, "\n");
    i32 ret = system(cmd);
    return ret >= 0;
}
