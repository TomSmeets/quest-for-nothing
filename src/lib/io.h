#pragma once
#include "types.h"

typedef struct {
    u32 cap;
    u32 write_index;
    u32 read_index;
    u8 *buffer;
} IO;

static bool io_can_read(IO *io) {
    return io->read_index < io->write_index;
}

static bool io_can_write(IO *io) {
    return io->write_index < io->cap;
}

static u8 io_read_u8(IO *io) {
    if(!io_can_read(io)) return 0;
    return io->buffer[io->read_index++];
}

static void io_write_u8(IO *io, u8 data) {
    if(!io_can_write(io)) return;
    io->buffer[io->write_index++] = data;
}

// Write a variable width unsigned integer
static void io_write_varint(IO *io, u64 value) {
    for(;;) {
        u8 byte = value & 0x7F;
        value >>= 7;
        if(value) byte |= 0x80;
        io_write_u8(io, byte);
        if(!value) break;
    }
}

// Read a variable width unsigned integer
static u64 io_read_varint(IO *io) {
    u64 value = 0;
    u32 shift = 0;
    for(;;) {
        u8 byte = io_read_u8(io);
        value |= (u64)(byte & 0x7F) << shift;
        shift += 7;
        if (!(byte & 0x80)) break;
    }
    return value;
}
