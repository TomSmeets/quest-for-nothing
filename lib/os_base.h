// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// os.h - Base platform API
#pragma once
#include "os_alloc.h"
#include "std.h"
#include "str.h"
#include "types.h"

static u64 os_time(void);
static void os_write(File *file, u8 *data, u32 len);
static void os_exit(i32 code);

#if OS_IS_LINUX
#include "os_base_linux.h"
#elif OS_IS_WINDOWS
#include "os_base_windows.h"
#elif OS_IS_WASM
#include "os_base_wasm.h"
#endif
