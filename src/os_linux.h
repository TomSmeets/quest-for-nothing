// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// os.h - Platform implementation for linux
#pragma once
#include "os.h"
#include "str.h"

#if 0
#include <dlfcn.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#else
// Improves compile time significantly
#define PROT_READ 0x1
#define PROT_WRITE 0x2
#define MAP_PRIVATE 0x02
#define MAP_ANONYMOUS 0x20
#define MAP_FAILED ((void *)-1)
#define CLOCK_MONOTONIC 1
#define RTLD_NOW 0x00002
#define RTLD_LOCAL 0
typedef long int time_t;
typedef long int syscall_slong_t;
typedef long int ssize_t;
typedef long unsigned int size_t;
typedef long int off_t;
typedef int clockid_t;
struct timespec {
    time_t tv_sec;
    syscall_slong_t tv_nsec;
};
extern void _exit(int status) __attribute__((__noreturn__));
extern ssize_t write(int fd, const void *buf, size_t n);
extern void *mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset);
extern int clock_gettime(clockid_t clock_id, struct timespec *tp);
extern int nanosleep(const struct timespec *__requested_time, struct timespec *__remaining);
extern void *dlopen(const char *file, int mode);
extern void *dlsym(void *restrict handle, const char *restrict name);
#endif

static u64 linux_time_to_us(struct timespec *t) {
    return t->tv_sec * 1000 * 1000 + t->tv_nsec / 1000;
}

static struct timespec linux_us_to_time(u64 time) {
    u64 sec = time / (1000 * 1000);
    u64 nsec = (time - sec * 1000 * 1000) * 1000;

    struct timespec ts;
    ts.tv_sec = sec;
    ts.tv_nsec = nsec;
    return ts;
}

static void os_exit(i32 status) {
    _exit(status);
}

static void log_error(char *message) {
    write(2, message, str_len(message));
}

static void os_fail(char *message) {
    write(2, message, str_len(message));
    _exit(1);
}

static void *os_alloc_raw(u32 size) {
    int prot = PROT_READ | PROT_WRITE;
    int flags = MAP_PRIVATE | MAP_ANONYMOUS;
    void *alloc = mmap(0, size, prot, flags, -1, 0);
    if (!alloc || alloc == MAP_FAILED) {
        log_error("Failed to allocate new memory\n");
        return 0;
    }
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
