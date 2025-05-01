// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// fs_types.h: Platform independent filesystem API
#pragma once
#include "types.h"
typedef struct FS_Dir FS_Dir;
struct FS_Dir {
    char *name;
    bool is_dir;
    FS_Dir *next;
};
