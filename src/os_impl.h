// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// os_impl.h - Automatic OS Platform implementation selector
#pragma once
#include "os.h"
#if OS_IS_LINUX
#include "os_linux.h"
#elif OS_IS_WINDOWS
#include "os_windows.h"
#elif OS_IS_WASM
#include "os_wasm.h"
#else
#error Unsupported platform
#endif
