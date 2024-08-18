#pragma once
#include "types.h"

#if 0
#include <dlfcn.h>
#include <fcntl.h>
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
    u32 mask;
    u32 cookie;
    u32 len;
    char name[];
};

extern int inotify_init(void);
extern int inotify_add_watch(int fd, const char *name, u32 mask);
#define IN_DELETE 0x00000200
#define IN_CREATE 0x00000100
#define IN_MODIFY 0x00000002
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
