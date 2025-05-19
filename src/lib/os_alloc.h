// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// os_alloc.h - Minimal OS functinality, allocate memory, and crash the program
#pragma once
#include "os_api.h"
#include "std.h"
#include "str.h"
#include "types.h"

// Allocate memory from the system
static void *os_alloc(u32 size);

// Kill the current process with an error message
static void os_fail(char *message);

#define TO_STRING0(x) #x
#define TO_STRING(x) TO_STRING0(x)
#define assert(cond, msg)                                                                                                                            \
    if (!(cond)) os_fail(__FILE__ ":" TO_STRING(__LINE__) ": assert(" #cond ") failed, " msg "\n")

// Assert wihtout a message
#define assert0(cond)                                                                                                                                \
    if (!(cond)) os_fail(__FILE__ ":" TO_STRING(__LINE__) ": assert(" #cond ") failed\n")

// Platform Implementations
#if OS_IS_LINUX
#include "os_alloc_linux.h"
#elif OS_IS_WINDOWS
#include "os_alloc_windows.h"
#elif OS_IS_WASM
#include "os_alloc_wasm.h"
#endif
