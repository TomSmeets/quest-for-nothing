// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// os_generic.h - Chooses the correct OS implementation to include
#pragma once

#if __unix__
#include "tlib/os_linux.h"
#define OS_LINUX 1
#elif _WIN32
#include "tlib/os_windows.h"
#define OS_WINDOWS 1
#else
static_assert(!"Unknown OS!");
#endif
