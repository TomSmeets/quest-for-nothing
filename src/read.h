// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// read.h - A simple line reader
#pragma once
#include "mem.h"
#include "os_main.h"

typedef struct {
    // Input file
    File *file;

    // The buffer
    u32 index;
    u32 size;
    u8 data[1024];
} Read;

static Read *read_new(Memory *mem, char *path) {
    Read *read = mem_struct(mem, Read);
    read->file = os_open(path, Open_Read);
    return read;
}

static void read_close(Read *read) {
    os_close(read->file);
}

// Read a single byte from the stream
static u8 read_next(Read *read) {
    if (read->index == read->size) {
        read->size = os_read(read->file, read->data, sizeof(read->data));
        read->index = 0;

        // nothing to read anymore
        if (read->size == 0) return 0;
    }

    return read->data[read->index++];
}

static char *read_line(Read *read, char *buffer, u32 size) {
    if (size == 0) return 0;

    u32 i = 0;
    for (;;) {
        u8 c = read_next(read);
        if (c == '\r') continue;

        // End of file
        if (c == '\0' && i == 0) return 0;

        // End of line
        if (c == '\n' || c == '\0') break;

        // Buffer is full
        if (i == size - 1) break;
        buffer[i++] = c;
    }

    // Append null byte
    buffer[i++] = 0;
    return buffer;
}
