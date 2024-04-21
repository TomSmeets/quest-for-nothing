// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// os_linux.h - linux platform implementation
#pragma once
#include "tlib/inc.h"
#include "tlib/mem.h"
#include "tlib/os.h"
#include "tlib/os_mem_caching.h"
#include "tlib/str.h"

#include <dirent.h>
#include <fcntl.h>        // File control, contains file options
#include <sys/mman.h>     // mmap
#include <sys/sendfile.h> // sendfile
#include <sys/stat.h>     // stat
#include <time.h>         // clock_gettime
#include <unistd.h>       // write

// ==== Main Entrypoint ====
int main(int argc, char *argv[]) {
    void *app = main_init(argc, argv);
    for (;;) {
        main_update(app);
    }
}

// ==== Basics ====
static void os_print(char *msg) {
    write(1, msg, str_len(msg));
}

static void os_error(char *msg) {
    write(2, msg, str_len(msg));
    __builtin_debugtrap();
    os_exit(1);
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
static Memory_Page *os_alloc_page_uncached(u64 size) {
    int prot = PROT_READ | PROT_WRITE;
    int flags = MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE;
    Memory_Page *page = mmap(0, size, prot, flags, -1, 0);
    assert(page && page != MAP_FAILED);
    page->size = size;
    page->next = 0;
    return page;
}

static void os_free_page_uncached(Memory_Page *page) {
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

    // sendfile limitation (2GB)
    assert(buffer_size <= 0x7ffff000);
    i64 ret = sendfile(dst_fd, src_fd, 0, buffer_size);
    assert(ret > 0);
    assert((u64)ret == buffer_size);
    close(src_fd);
    close(dst_fd);
}

static Buffer os_read_file(Memory *m, char *path) {
    assert(path);

    struct stat sb;
    if (stat(path, &sb) != 0) {
        // Does not exist
        return (Buffer){0};
    }
    if (!S_ISREG(sb.st_mode)) {
        // Not a file
        return (Buffer){0};
    }
    assert(sb.st_size >= 0);

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
    return (Buffer){buffer, buffer_size};
}

static void os_write_file(char *path, Buffer data) {
    assert(path);

    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    i64 n_written = write(fd, data.ptr, data.size);
    assert(n_written >= 0);
    assert((u64)n_written == data.size);
    close(fd);
}

static u64 os_file_mtime(char *path) {
    struct stat sb = {};
    stat(path, &sb);
    return linux_time_to_u64(&sb.st_mtim);
}

static os_dir *os_read_dir(Memory *m, char *path) {
    os_dir *first = 0;
    os_dir *last = 0;

    DIR *c_dir = opendir(path);
    for (;;) {
        struct dirent *ent = readdir(c_dir);

        // check if done
        if (!ent)
            break;

        char *file_name = ent->d_name;

        // skip these
        if (str_eq(file_name, "."))
            continue;
        if (str_eq(file_name, ".."))
            continue;

        os_dir *dir = mem_struct(m, os_dir);
        dir->file_name = str_dup(m, file_name);
        dir->is_file = ent->d_type == DT_REG;
        dir->is_dir = ent->d_type == DT_DIR;
        dir->is_link = ent->d_type == DT_LNK;
        list_append(first, last, dir);
    }
    closedir(c_dir);

    return first;
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
