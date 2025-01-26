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
#include <sys/inotify.h>
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
extern void _exit(int status) __attribute__((__noreturn__));
extern int open(const char *file, int oflag, ...);
extern ssize_t write(int fd, const void *buf, size_t n);
extern ssize_t read(int fd, void *buf, size_t n);
extern int close(int fd);
extern void *mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset);
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

typedef long int fd_mask;
#define FD_BIT_COUNT (8 * sizeof(fd_mask))
typedef struct {
    fd_mask bits[1024 / FD_BIT_COUNT];
} fd_set;
#define FD_ELT(fd) ((fd) / FD_BIT_COUNT)
#define FD_MASK(fd) (1UL << ((fd) % FD_BIT_COUNT))
#define FD_SET(fd, set) (set)->bits[FD_ELT(fd)] |= FD_MASK(fd)
#define FD_ISSET(fd, set) (((set)->bits[FD_ELT(fd)] & FD_MASK(fd)) != 0)
extern int select(int nfds, fd_set *restrict readfds, fd_set *restrict writefds, fd_set *restrict exceptfds, struct timeval *restrict timeout);
#define NAME_MAX 255

struct inotify_event {
    int wd;
    unsigned int mask;
    unsigned int cookie;
    unsigned int len;
    char name[];
};

extern int inotify_init(void);
extern int inotify_add_watch(int fd, const char *name, unsigned int mask);
#define IN_DELETE 0x00000200
#define IN_CREATE 0x00000100
#define IN_MODIFY 0x00000002
#endif

static i64 linux_syscall6(i64 a0, i64 a1, i64 a2, i64 a3, i64 a4, i64 a5, i64 a6) {
    i64 ret;
    // Registers
    register i64 r0 __asm__("rax") = a0;
    register i64 r1 __asm__("rdi") = a1;
    register i64 r2 __asm__("rsi") = a2;
    register i64 r3 __asm__("rdx") = a3;
    register i64 r4 __asm__("r10") = a4;
    register i64 r5 __asm__("r9") = a5;
    register i64 r6 __asm__("r8") = a6;

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
    register i64 r5 __asm__("r9") = a5;
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

__attribute__((__noreturn__)) static void linux_exit_group(i32 error_code) {
    for (;;) linux_syscall1(0xe7, error_code);
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
// clone3({flags=CLONE_VM|CLONE_VFORK|CLONE_CLEAR_SIGHAND, exit_signal=SIGCHLD, stack=0x78f294890000, stack_size=0x9000}, 88 <unfinished ...>
// execve("/bin/sh", ["sh", "-c", "--", "ls -la"], 0x7ffe3289e8e8 /* 50 vars */ <unfinished ...>


703657 rt_sigaction(SIGINT, {sa_handler=SIG_IGN, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x794ed82491d0}, {sa_handler=SIG_DFL, sa_mask=[], sa_flags=0}, 8) = 0
703657 rt_sigaction(SIGQUIT, {sa_handler=SIG_IGN, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x794ed82491d0}, {sa_handler=SIG_DFL, sa_mask=[], sa_flags=0}, 8) = 0
703657 rt_sigprocmask(SIG_BLOCK, [CHLD], [], 8) = 0
703657 mmap(NULL, 36864, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_STACK, -1, 0) = 0x794ed8430000
703657 rt_sigprocmask(SIG_BLOCK, ~[], [CHLD], 8) = 0
703657 clone3({flags=CLONE_VM|CLONE_VFORK|CLONE_CLEAR_SIGHAND, exit_signal=SIGCHLD, stack=0x794ed8430000, stack_size=0x9000}, 88 <unfinished ...>
703658 rt_sigprocmask(SIG_BLOCK, NULL, ~[KILL STOP], 8) = 0
703658 rt_sigaction(SIGINT, {sa_handler=SIG_DFL, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x794ed82491d0}, NULL, 8) = 0
703658 rt_sigaction(SIGQUIT, {sa_handler=SIG_DFL, sa_mask=[], sa_flags=SA_RESTORER, sa_restorer=0x794ed82491d0}, NULL, 8) = 0
703658 rt_sigprocmask(SIG_SETMASK, [], NULL, 8) = 0
703658 execve("/bin/sh", ["sh", "-c", "--", "ls -la"], 0x7fff316b0608 /* 50 vars */ <unfinished ...>
703657 <... clone3 resumed>)            = 703658
}
#endif
