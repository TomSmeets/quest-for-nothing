// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// os_alloc.h - Minimal OS functinality, allocate memory, and crash the program
#pragma once
#include "std.h"
#include "str.h"
#include "os_api.h"
#include "types.h"

// Allocate memory from the system
static void *os_alloc(u32 size);

// Kill the current process with an error message
static void os_fail(char *message);

#define assert(cond, msg)                                                                                                                            \
    if (!(cond)) os_fail("Assertion failed: " #cond " " msg "\n")

// Platform Implementations
#if OS_IS_LINUX
#include "os_alloc_linux.h"
#endif

#if OS_IS_WINDOWS
#include "os_alloc_windows.h"
#endif

#if OS_IS_WASM
#include "os_alloc_wasm.h"
#endif
