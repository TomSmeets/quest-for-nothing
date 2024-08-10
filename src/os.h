// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// os.h - Platform api
#pragma once
#include "os_api.h"
#include "types.h"

#if __linux__
#define OS_IS_LINUX 1
#include "os_linux.h"
#endif
