// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// fs.h: Platform independent filesystem API
#pragma once
#include "fs_types.h"
#include "mem.h"
#include "os_api.h"

static FS_Dir *fs_list(struct Memory *mem, char *path);
static void fs_remove(char *path);
static void fs_mkdir(char *path);

#if OS_IS_LINUX
#include "fs_linux.h"
#else
#error FS is not implemented for this platform
#endif
