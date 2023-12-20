// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// os_linux.h - linux platform implementation
#pragma once
#include "inc.h"
#include "mem.h"
#include "os.h"
#include "str.h"

#include <fcntl.h> // File control, contains file options
#include <sys/mman.h> // mmap
#include <sys/sendfile.h> // sendfile
#include <sys/stat.h> // stat
#include <time.h>     // clock_gettime
#include <unistd.h>   // write

// ==== Main Entrypoint ====
int main(int argc, char *argv[]) {
    void *app = main_init(argc, argv);
    for(;;) { main_update(app); }
}

// ==== Basics ====
static void os_print(char *msg) {
    write(1, msg, str_len(msg));
}

static void os_assert(bool cond, char *msg) {
    if (cond) return;
    write(2, msg, str_len(msg));
    __builtin_debugtrap();
}

static void os_exit(u32 code) {
    _exit(code);
}

static u64 linux_time_to_u64(struct timespec *t) {
    return t->tv_sec * 1000 * 1000 + t->tv_nsec / 1000;
}

static u64 os_time(void) {
    struct timespec t = {};
    assert(clock_gettime(CLOCK_MONOTONIC, &t) == 0);
    return linux_time_to_u64(&t);
}

static void os_sleep_until(u64 time) {
    u64 current_time = os_time();
    if (time > current_time)
        usleep(time - current_time);
}

// ==== Memory ====
#define OS_ALLOC_INC (1 * SIZE_MB)

// It is a cache, so we can just clear it on a hot reload
static mem_page *os_page_cache = 0;

// Allocate a memory page of at least 'size' bytes, or some page increment
static mem_page *os_alloc_page(u64 size) {
    if (size <= OS_ALLOC_INC)
        size = OS_ALLOC_INC;

    // use a previously allocated page
    if (os_page_cache && size == OS_ALLOC_INC) {
        mem_page *p = os_page_cache;
        os_page_cache = p->next;
        p->next = 0;
        assert(p->size == OS_ALLOC_INC);
        return p;
    }

    int prot = PROT_READ | PROT_WRITE;
    int flags = MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE;
    mem_page *page = mmap(0, size, prot, flags, -1, 0);
    assert(page && page != MAP_FAILED);
    page->size = size;
    page->next = 0;
    return page;
}

static void os_free_page(mem_page *page) {
    // put cached pages back into the cache;
    if (page->size == OS_ALLOC_INC) {
        page->next = os_page_cache;
        os_page_cache = page;
        return;
    }

    assert(munmap(page, page->size) == 0);
}

// ==== Filesystem ====
static void os_copy_file(char *src, char *dst) {
    assert(src);
    assert(dst);

    struct stat sb;
    assert(stat(src, &sb) == 0);
    assert(sb.st_size >= 0);
    assert(S_ISREG(sb.st_mode));
    u64 buffer_size = (u64)sb.st_size;

    bool executable = str_ends_with(dst, ".so");
    int src_fd = open(src, O_RDONLY);
    int dst_fd = open(dst, O_CREAT | O_WRONLY | O_TRUNC, executable ? 0755 : 0644);
    assert(src_fd >= 0);
    assert(dst_fd >= 0);

    // sendfine limitation (2GB)
    assert(buffer_size <= 0x7ffff000);
    i64 ret = sendfile(dst_fd, src_fd, 0, buffer_size);
    assert(ret > 0);
    assert((u64)ret == buffer_size);
    close(src_fd);
    close(dst_fd);
}

static buf os_read_file(mem *m, char *path) {
    assert(path);
    struct stat sb;
    assert(stat(path, &sb) == 0);
    assert(sb.st_size >= 0);
    assert(S_ISREG(sb.st_mode));

    int fd = open(path, O_RDONLY);
    u64 buffer_size = (u64)sb.st_size;

    // also reserve one byte for zero termination
    u8 *buffer = mem_push(m, buffer_size + 1);
    i64 n_read = read(fd, buffer, buffer_size);
    assert(n_read >= 0);
    assert((u64)n_read == buffer_size);
    close(fd);

    // zero terminate
    buffer[buffer_size] = 0;
    return (buf){buffer, buffer_size};
}

static u64 os_file_mtime(char *path) {
    struct stat sb = {};
    stat(path, &sb);
    return linux_time_to_u64(&sb.st_mtim);
}

// ==== Dynamic Libraries ====
extern void *dlopen(const char *file, int mode);
extern void *dlsym(void *handle, const char *name);

#define RTLD_LAZY 0x00001
static void *os_dlopen(char *file) {
    return dlopen(file, RTLD_LAZY);
}

static void *os_dlsym(void *handle, char *name) {
    return dlsym(handle, name);
}

// ==== Processes ====
extern int system(const char *cmd);
static i32 os_command(char *cmd) {
    return system(cmd);
}
