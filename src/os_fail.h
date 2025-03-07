#pragma once
#include "os_api.h"

// Kill the current process with an error message
static void os_fail(char *message);

#define assert(cond, msg)                                                                                                                            \
    if (!(cond)) os_fail("Assertion failed: " #cond " " msg "\n")

#if OS_IS_LINUX
static void os_fail(char *message) {
    linux_write(1, message, str_len(message));
    linux_exit_group(1);
}

#elif OS_IS_WINDOWS
static void os_fail(char *message) {
    MessageBox(NULL, message, "Error", MB_ICONERROR | MB_OK);
}

#elif OS_IS_WASM
WASM_IMPORT(wasm_fail) void wasm_fail(char *);
static void os_fail(char *message) {
    wasm_fail(message);
}
#endif
