// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// os.h - Base platform API
#pragma once
#include "types.h"

#if __unix__
#define OS_IS_LINUX 1
#else
#define OS_IS_LINUX 0
#endif

#if _WIN32
#define OS_IS_WINDOWS 1
#else
#define OS_IS_WINDOWS 0
#endif

#if __wasm__
#define OS_IS_WASM 1
#else
#define OS_IS_WASM 0
#endif

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

    // == Statistics ==
    u32 stat_alloc_count;
    u32 stat_alloc_size;

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
static void os_write(File *file, u8 *data, u32 len);
static void os_exit(i32 code);
static void os_fail(char *message);
static void *os_alloc_raw(u32 size);

// === Desktop API ===
typedef enum {
    Open_Write,
    Open_Read,
    Open_Dir,
} OS_Open_Type;

static File *os_open(char *path, OS_Open_Type type);
static void os_close(File *file);

static u32 os_read(File *file, u8 *data, u32 len);
static void os_sleep(u64 time);

static File *os_dlopen(char *path);
static void *os_dlsym(File *handle, char *name);
static char *os_dlerror(void);

static bool os_system(char *command);

// Set maximum wait time between os_main calls
static void os_set_update_time(u64 wake_time) {
    OS *os = OS_GLOBAL;
    if (os->sleep_time > wake_time) {
        os->sleep_time = wake_time;
    }
}
