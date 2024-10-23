// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// os.h - Generic platform API
#pragma once
#include "fmt.h"
#include "mem.h"
#include "os_api.h"

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

#if OS_IS_LINUX
#include "os_linux.h"
#elif OS_IS_WINDOWS
#include "os_windows.h"
#elif OS_IS_WASM
#include "os_wasm.h"
#else
#error Unsupported platform
#endif

static void os_print(char *message) {
    os_write(os_stdout(), (u8 *)message, str_len(message));
}

// Create the OS structure.
//
// A typical main entry point should look like this:
// >> int main(int argc, char **argv) {
// >>     OS *os = os_init(argc, argv);
// >>     for (;;) {
// >>         os_main(os);
// >>         os_sleep(os->sleep_time);
// >>     }
// >> }
static OS *os_init(int argc, char **argv) {
    Memory *mem = mem_new();
    OS *os = mem_struct(mem, OS);
    os->argc = argc;
    os->argv = argv;
    os->fmt = fmt_new(mem, os_stdout());
    OS_GLOBAL = os;
    return os;
}
