// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// os_api.h - Platform api
#pragma once
#include "types.h"
#include "input_type.h"
#include "mat.h"

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

    // Time to sleep until next call
    u64 sleep_time;

    // Cached allocations (handled by os)
    OS_Alloc *cache;
    void *sdl2_handle;

    // Unique id generation
    u32 uid;
} OS;

// The only global variable allowed
static OS *OS_GLOBAL;

// Callbacks
static void os_main(OS *os);
static void os_audio(OS *os, u32 count, v2 *samples);

// Read
static u64 os_time(void);
static u64 os_rand(void);

// Actions
static void os_write(u8 *data, u32 len);
static void os_exit(i32 code);
static void os_fail(char *message);
static void *os_alloc_raw(u32 size);

// Graphics
static void os_gfx_init(char *title);
static Input *os_gfx_begin(void);
static void os_gfx_quad(m4s mtx);
static void os_gfx_end(void);

