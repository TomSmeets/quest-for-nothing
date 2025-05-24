// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// read.h - A simple line reader
#pragma once
#include "mem.h"
#include "os_desktop.h"
#include "types.h"

typedef struct {
    // Input file
    File *file;

    // The buffer
    u32 index;
    u32 size;
    u8 data[1024];
} Read;

static Read *read_new(Memory *mem, String path) {
    Read *read = mem_struct(mem, Read);
    read->file = os_open(path, Open_Read);
    return read;
}

static void read_close(Read *read) {
    os_close(read->file);
}

// Fill read buffer if needed
// Returns true when there is still data left
// returns false when the buffer is finished
static bool read_fill(Read *read) {
    if(read->index < read->size) return true;
    read->size = os_read(read->file, read->data, sizeof(read->data));
    read->index = 0;
    return read->size > 0;
}

// Check if the read is done
static bool read_eof(Read *read) {
    return read_fill(read);
}

// Read a single byte from the stream
static u8 read_u8(Read *read) {
    if(!read_fill(read)) return 0;
    return read->data[read->index++];
}

static void read_buf(Read *read, u32 len, u8 *data) {
    for (u32 i = 0; i < len; ++i) data[i] = read_u8(read);
}

static bool read_match(Read *read, String match) {
    for (u32 i = 0; i < match.len; ++i) {
        u8 c = read_u8(read);
        if(c != match.data[i]) return false;
    }
    return true;
}

static u32 read_u32be(Read *read) {
    u32 result = 0;
    result |= read_u8(read) << (8*3);
    result |= read_u8(read) << (8*2);
    result |= read_u8(read) << (8*1);
    result |= read_u8(read) << (8*0);
    return result;
}

static u32 read_varint_be(Read *read) {
    u32 result = 0;
    for(;;) {
        u8 c = read_u8(read);
        u8 next = c & 0b10000000;
        u8 data = c & 0b01111111;
        result <<= 7;
        result |= data;
        if (!next) break;
    }
    return result;
}

static char *read_line(Read *read, char *buffer, u32 size) {
    if (size == 0) return 0;

    u32 i = 0;
    for (;;) {
        u8 c = read_u8(read);
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
