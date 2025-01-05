// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// gfx_impl.h - Automatic GFX Platform implementation selector
#pragma once
#include "gfx.h"
#include "os.h"
#if OS_IS_LINUX || OS_IS_WINDOWS
#include "gfx_desktop.h"
#elif OS_IS_WASM
#include "gfx_wasm.h"
#else
#error Unsupported platform
#endif
