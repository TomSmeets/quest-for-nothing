// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// read.h - A simple line reader
#pragma once
#include "str.h"
#include "types.h"

// Check if the read is done
static bool read_eof(String *read) {
    return read->len == 0;
}

static u8 read_u8(String *read) {
    if (read->len == 0) return 0;
    u8 byte = read->data[0];
    read->data++;
    read->len--;
    return byte;
}

static String read_buf(String *read, u32 len) {
    if (len > read->len) len = read->len;
    if (len == 0) return S0;

    String left = {len, read->data};
    read->data += len;
    read->len -= len;
    return left;
}

static u32 read_u32be(String *read) {
    u32 result = 0;
    result |= read_u8(read) << (8 * 3);
    result |= read_u8(read) << (8 * 2);
    result |= read_u8(read) << (8 * 1);
    result |= read_u8(read) << (8 * 0);
    return result;
}

static u32 read_varint(String *read) {
    u32 result = 0;
    for (;;) {
        u8 c = read_u8(read);
        u8 next = c & 0b10000000;
        u8 data = c & 0b01111111;
        result <<= 7;
        result |= data;
        if (!next) break;
    }
    return result;
}

static String read_line(String *read) {
    u32 index = str_find(*read, '\n');
    String result = {.data = read->data, .len = index};

    read->len -= index;
    read->data += index;
    return result;
}
