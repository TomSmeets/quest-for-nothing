// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// os_generic.h - Chooses the correct OS implmentation to include
#pragma once

#if __unix__
#include "os_linux.h"
#elif  _WIN32
#include "os_windows.h"
#else
static_assert(!"Unknown OS!");
#endif
