// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// fs_api.h: Platform independent filesystem API interface
#pragma once
#include "lib/mem.h"
#include "lib/str.h"

// Directory listing result
typedef struct FS_Dir FS_Dir;
struct FS_Dir {
    String name;
    bool is_dir;
    FS_Dir *next;
};

typedef enum {
    FS_Open_Read,
    FS_Open_Write,
    FS_Open_Directory,
} FS_Open_Type;

// Open a file for reading or writing
// If type is write and the file does not exist create the file
static File *fs_open(String path, FS_Open_Type type);

// Close opened file and flush changes to the filesystem
static void fs_close(File *file);

// Remove a file
// Returns false on failure
static bool fs_remove(String path);

// Create directory if not yet exists
// Returns false on failure
static bool fs_mkdir(String path);

// Remove an empty directory
static bool fs_rmdir(String path);

// List files in directory
// Returns null on failure
static FS_Dir *fs_list(Memory *mem, String path);
