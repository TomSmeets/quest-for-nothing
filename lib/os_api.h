// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// os_api.h - Platform syscall headers and platform detection
#pragma once

// Linux
#if __unix__
#define OS_IS_LINUX 1
#define OS_IS_WINDOWS 0
#define OS_IS_WASM 0
#include "os_api_linux.h"
#endif

// Windows
#if _WIN32
#define OS_IS_LINUX 0
#define OS_IS_WINDOWS 1
#define OS_IS_WASM 0
#include "os_api_windows.h"
#endif

// Webassembly
#if __wasm__
#define OS_IS_LINUX 0
#define OS_IS_WINDOWS 0
#define OS_IS_WASM 1
#include "os_api_wasm.h"
#endif
