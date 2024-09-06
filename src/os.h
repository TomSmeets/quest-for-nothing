// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// os.h - Generic platform API
#pragma once
#include "os_api.h"
#include "str.h"
#include "types.h"

#if __unix__
#define OS_IS_LINUX 1
#include "os_linux.h"
#elif _WIN32
#define OS_IS_WINDOWS 1
#include "os_windows.h"
#elif __wasm__
#define OS_IS_WASM 1
#include "os_wasm.h"
#else
#error Did not recognize platform
#endif

