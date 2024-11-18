// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// gfx.h - Graphics, Audio, and Input handling abstraction implementation
#pragma once
#include "gfx_api.h"
#include "os_api.h"

#if OS_IS_LINUX || OS_IS_WINDOWS
#include "gfx_desktop.h"
#elif OS_IS_WASM
#include "gfx_wasm.h"
#endif
