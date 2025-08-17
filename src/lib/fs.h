// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// fs.h: Platform independent filesystem API
#pragma once
#include "lib/fs_api.h"

#if OS_IS_LINUX
#include "lib/fs_linux.h"
#elif OS_IS_WINDOWS
#include "lib/fs_windows.h"
#else
#error FS is not implemented for this platform
#endif
