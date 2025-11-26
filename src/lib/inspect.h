#pragma once
#include "io.h"
#include "mem.h"
#include "str.h"
#include "types.h"

static u32 DEBUG_DATA[4 * 1024] = {
    0x452307a1,         // magick 1
    0x4cae5cf0,         // magick 2
    sizeof(DEBUG_DATA), // max size
};

typedef enum {
    Root,
} Inspect_Tag;

typedef struct Inspect_Value Inspect_Value;
struct Inspect_Value {
    Inspect_Tag tag;
    String name;
    u64 value;
    u32 child_count;
    Inspect_Value *children;
};

static Inspect_Value *parse(Memory *mem) {
    IO io = {
        .buffer = (u8 *)DEBUG_DATA,
        .cap = sizeof(DEBUG_DATA),
        .write_index = sizeof(DEBUG_DATA),
    };

    // Skip header
    io.read_index += 4 * 4;
    u32 count = io_read_varint(&io);
    Inspect_Value *values = mem_array_zero(mem, Inspect_Value, count);
    for (u32 i = 0; i < count; ++i) values[i].tag = io_read_varint(&io);
    for (u32 i = 0; i < count; ++i) values[i].name.len = io_read_varint(&io);
    for (u32 i = 0; i < count; ++i) {
        values[i].name.data = io.buffer + io.read_index;
        io.read_index += values[i].name.len;
    }
    return &values[0];
}
