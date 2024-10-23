// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// os_api.h - Base platform api
#pragma once
#include "types.h"

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

    // == Globals for other modules ==

    // Cached memory allocations, see mem.h
    void *memory_cache;

    // Global formatter for printing to stdout, see fmt.h
    void *fmt;

    // Counter for unique id generation, see id.h
    u32 uid;
} OS;

typedef struct File File;

// The only global variable
static OS *OS_GLOBAL;

// Callbacks
static void os_main(OS *os);

// Read
static u64 os_time(void);
static u64 os_rand(void);

// Actions
static File *os_stdout(void);
static void os_write(File *file, u8 *data, u32 len);
static void os_exit(i32 code);
static void os_fail(char *message);
static void *os_alloc_raw(u32 size);

// Derived
static void *os_alloc(void);
static void os_free(void *ptr);

// Create
static OS *os_init(int argc, char **argv);
static void os_print(char *message);
