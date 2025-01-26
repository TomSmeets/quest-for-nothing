#pragma once
#include "types.h"

static_assert(sizeof(long) == sizeof(i64));
static_assert(sizeof(int) == sizeof(i32));
static_assert(sizeof(void *) == sizeof(u64));
// u32 mode_t
// u64 size_t

#if 1
#define _POSIX_C_SOURCE 199309L
#define _DEFAULT_SOURCE
#include <dirent.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
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
#define O_RDONLY 00
#define O_WRONLY 01
#define O_RDWR 02
#define O_CREAT 0100
#define O_TRUNC 01000
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
extern int clock_gettime(clockid_t clock_id, struct timespec *tp);
extern int nanosleep(const struct timespec *__requested_time, struct timespec *__remaining);

// dlfcn..h
extern void *dlopen(const char *file, int mode);
extern void *dlsym(void *restrict handle, const char *restrict name);
extern char *dlerror(void);

extern int system(const char *command);
struct timeval {
    time_t tv_sec;
    syscall_slong_t tv_usec;
};

#define NAME_MAX 255
#endif

typedef struct {
    u64 bits[1024 / (8 * sizeof(u64))];
} linux_fd_set;

static i64 linux_syscall6(i64 a0, i64 a1, i64 a2, i64 a3, i64 a4, i64 a5, i64 a6) {
    i64 ret;
    // Registers
    register i64 r0 __asm__("rax") = a0;
    register i64 r1 __asm__("rdi") = a1;
    register i64 r2 __asm__("rsi") = a2;
    register i64 r3 __asm__("rdx") = a3;
    register i64 r4 __asm__("r10") = a4;
    register i64 r5 __asm__("r8") = a5;
    register i64 r6 __asm__("r9") = a6;

    // https://stackoverflow.com/a/54957101
    // https://gitlab.com/x86-psABIs/x86-64-ABI
    __asm__ __volatile__(
        // Instruction
        "syscall"
        // Output registers a -> rax, '=' indicates write only
        // https://gcc.gnu.org/onlinedocs/gcc/Extended-Asm.html#Output-Operands
        // https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html
        : "=a"(ret)
        : "r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5), "r"(r6)
        // Clobbers, these are not preserved
        : "rcx", "r11", "memory"
    );
    return ret;
}

// Same but fewer arguments, see linux_syscall6
static i64 linux_syscall5(i64 a0, i64 a1, i64 a2, i64 a3, i64 a4, i64 a5) {
    i64 ret;
    register i64 r0 __asm__("rax") = a0;
    register i64 r1 __asm__("rdi") = a1;
    register i64 r2 __asm__("rsi") = a2;
    register i64 r3 __asm__("rdx") = a3;
    register i64 r4 __asm__("r10") = a4;
    register i64 r5 __asm__("r8") = a5;
    __asm__ __volatile__("syscall" : "=a"(ret) : "r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5) : "rcx", "r11", "memory");
    return ret;
}

// Same but fewer arguments, see linux_syscall6
static i64 linux_syscall4(i64 a0, i64 a1, i64 a2, i64 a3, i64 a4) {
    i64 ret;
    register i64 r0 __asm__("rax") = a0;
    register i64 r1 __asm__("rdi") = a1;
    register i64 r2 __asm__("rsi") = a2;
    register i64 r3 __asm__("rdx") = a3;
    register i64 r4 __asm__("r10") = a4;
    __asm__ __volatile__("syscall" : "=a"(ret) : "r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4) : "rcx", "r11", "memory");
    return ret;
}

// Same but fewer arguments, see linux_syscall6
static i64 linux_syscall3(i64 a0, i64 a1, i64 a2, i64 a3) {
    i64 ret;
    register i64 r0 __asm__("rax") = a0;
    register i64 r1 __asm__("rdi") = a1;
    register i64 r2 __asm__("rsi") = a2;
    register i64 r3 __asm__("rdx") = a3;
    __asm__ __volatile__("syscall" : "=a"(ret) : "r"(r0), "r"(r1), "r"(r2), "r"(r3) : "rcx", "r11", "memory");
    return ret;
}

// Same but fewer arguments, see linux_syscall6
static i64 linux_syscall2(i64 a0, i64 a1, i64 a2) {
    i64 ret;
    register i64 r0 __asm__("rax") = a0;
    register i64 r1 __asm__("rdi") = a1;
    register i64 r2 __asm__("rsi") = a2;
    __asm__ __volatile__("syscall" : "=a"(ret) : "r"(r0), "r"(r1), "r"(r2) : "rcx", "r11", "memory");
    return ret;
}

// Same but fewer arguments, see linux_syscall6
static i64 linux_syscall1(i64 a0, i64 a1) {
    i64 ret;
    register i64 r0 __asm__("rax") = a0;
    register i64 r1 __asm__("rdi") = a1;
    __asm__ __volatile__("syscall" : "=a"(ret) : "r"(r0), "r"(r1) : "rcx", "r11", "memory");
    return ret;
}

// Same but fewer arguments, see linux_syscall6
static i64 linux_syscall0(i64 a0) {
    i64 ret;
    register i64 r0 __asm__("rax") = a0;
    __asm__ __volatile__("syscall" : "=a"(ret) : "r"(r0) : "rcx", "r11", "memory");
    return ret;
}

// Sys calls
static i64 linux_read(i32 fd, void *buf, u64 size) {
    return linux_syscall3(0x00, fd, (i64)buf, size);
}

static i64 linux_write(i32 fd, const void *buf, u64 size) {
    return linux_syscall3(0x01, fd, (i64)buf, size);
}

static i32 linux_open(const char *path, i32 flags, u32 mode) {
    return linux_syscall3(0x02, (i64)path, flags, mode);
}

static i32 linux_close(i32 fd) {
    return linux_syscall1(0x03, fd);
}

static i64 linux_getrandom(void *buf, u64 size, u32 flags) {
    return linux_syscall3(0x13e, (i64)buf, size, flags);
}

__attribute__((__noreturn__)) static void linux_exit_group(i32 error_code) {
    for (;;) linux_syscall1(0xe7, error_code);
}

static void *linux_mmap(void *addr, u64 len, i32 prot, i32 flags, i32 fd, i64 offset) {
    return (void *)linux_syscall6(0x09, (i64)addr, len, prot, flags, fd, offset);
}

static i32 linux_select(i32 count, linux_fd_set *input_fds, linux_fd_set *output_fds, linux_fd_set *except_fds, struct timeval *timeout) {
    return linux_syscall5(0x17, count, (i64)input_fds, (i64)output_fds, (i64)except_fds, (i64)timeout);
}

static i32 linux_nanosleep(const struct timespec *duration, struct timespec *remaining) {
    return linux_syscall2(0x23, (i64)duration, (i64)remaining);
}

#define IN_DELETE 0x00000200 // File was modified
#define IN_CREATE 0x00000100 // Subfile was created
#define IN_MODIFY 0x00000002 // Subfile was deleted

struct inotify_event {
    i32 wd;
    u32 mask;
    u32 cookie;
    u32 len;
    char name[];
};

static i32 linux_inotify_init(i32 flags) {
    return linux_syscall1(0x126, flags);
}

static i32 linux_inotify_add_watch(i32 fd, const char *path, u32 mask) {
    return linux_syscall3(0xfe, fd, (i64)path, mask);
}

#if 0
struct linux_dirent64 {
    u64 d_ino;
    i64 d_off;
    unsigned short d_reclen;
    unsigned char d_type;
    char d_name[];
};

static i64 linux_getdents64(u32 fd, struct linux_dirent64 *dirent, unsigned int count) {
    return linux_syscall3(0xd9, fd, (i64)dirent, count);
}

struct linux_clone_args {
    u64 flags;
    i32 *pidfd;
    i32 *child_tid;
    i32 *parent_tid;
    u64 exit_signal;
    void * stack;
    u64 stack_size;
    u64 tls;
    u64 set_tid;
    u64 set_tid_size;
    u64 cgroup;
};

static_assert(sizeof(struct linux_clone_args) == sizeof(u64) * 11);

static i64 linux_clone3(struct linux_clone_args *args){
    return linux_syscall2(0x15a, (i64) args, sizeof(*args));
}

static void linux_spawn(u32 argc, char *argv) {
    struct linux_clone_args args = {};
    args.flags = CLONE_VM | CLONE_VFORK | SIGCHLD;
    arg.stack = mmap(0, 36*1024, 
    linux_clone3(args)
}
#endif
