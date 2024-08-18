// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// os_api.h - Platform api
#pragma once
#include "types.h"

#define OS_ALLOC_SIZE (1024 * 1024)

// A block of memory. total size = OS_ALLOC_SIZE
typedef struct OS_Alloc {
    struct OS_Alloc *next;
} OS_Alloc;

typedef struct {
    // Application handle
    void *app;

    // Command line args
    u32 argc;
    char **argv;

    // Was the application reloaded
    bool reloaded;

    // Cached allocations (handled by os)
    OS_Alloc *cache;
    void *sdl2_handle;
    u64 uid;
} OS;

// The only global variable allowed
static OS *OS_GLOBAL;

static void os_main(OS *os);

// Platform API
#define OS_STDIN 0
#define OS_STDOUT 1
static void os_write(u32 fd, u8 *data, u32 len);

static void os_exit(i32 code);
static void os_fail(char *message);

static void *os_alloc_raw(u32 size);

static u64 os_time(void);
static void os_sleep(u64 time);

// load a sdl2 symbol
static void *os_load_sdl2(char *name);

// Generic api
static void os_print(char *message);

static OS_Alloc *os_alloc(void);
static void os_free(OS_Alloc *ptr);

static u64 os_rand(void);
