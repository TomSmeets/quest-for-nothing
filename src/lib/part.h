// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// part.h - Text part
#pragma once
#include "mem.h"
#include "types.h"

typedef struct Part Part;
struct Part {
    Part *next;
    u8 len;
    u8 data[128];
};

static Part *part_new(Memory *mem) {
    Part *part = mem_struct_uninit(mem, Part);
    part->next = 0;
    part->len = 0;
    return part;
}

static void part_free(Memory *mem, Part *part) {
    // mem_free_struct(mem, part);
}

// Fill part with data from str
// returns newly inserted string
static String part_fill(Part *part, String str) {
    u32 len = str.len;
    u32 size_left = array_count(part->data) - part->len;
    if (len > size_left) len = size_left;

    String dst = {
        .len = len,
        .data = part->data + part->len,
    };
    std_memcpy(dst.data, str.data, dst.len);
    part->len += len;
    return dst;
}

// Split part at index
// Reduces 'left' size to match offset
// moves data to a new allocated 'right' part
// Returns 'right' part
static Part *part_split(Memory *mem, Part *left, u32 offset) {
    u32 len_left = offset;
    u32 len_right = left->len - offset;

    Part *right = part_new(mem);
    std_memcpy(right->data, left->data + len_left, len_right);
    left->len = len_left;
    right->len = len_right;
    return right;
}

// Move all data from right to left, if it fits
// Otherwise don't move and return false
// This is the inverse of 'split'
static bool part_join(Part *left, Part *right) {
    if (left->len + right->len > sizeof(left->data)) return false;
    std_memcpy(left->data + left->len, right->data, right->len);
    left->len += right->len;
    right->len = 0;
    return true;
}

static void part_delete(Part *part, u32 start, u32 count) {
    assert0(start + count <= part->len);
    u32 end = start + count;
    std_memmove(part->data + start, part->data + end, part->len - end);
    part->len -= count;
}

static String part_to_str(Part *part) {
    return (String){part->len, part->data};
}

static void test_part(void) {
    Memory *mem = mem_new();

    Part *left = part_new(mem);

    String str = S("Hello World");

    String buf = part_fill(left, str);
    assert0(buf.len == str.len);
    assert0(left->len == str.len);
    assert0(str_eq(str, part_to_str(left)));

    Part *right = 0;

    right = part_split(mem, left, 0);
    assert0(str_eq(part_to_str(left), S0));
    assert0(str_eq(part_to_str(right), S("Hello World")));
    assert0(part_join(left, right));
    assert0(str_eq(part_to_str(left), S("Hello World")));
    assert0(str_eq(part_to_str(right), S0));
    part_free(mem, right);

    right = part_split(mem, left, 5);
    assert0(str_eq(part_to_str(left), S("Hello")));
    assert0(str_eq(part_to_str(right), S(" World")));
    assert0(part_join(left, right));
    assert0(str_eq(part_to_str(left), S("Hello World")));
    assert0(str_eq(part_to_str(right), S0));
    part_free(mem, right);

    right = part_split(mem, left, str.len);
    assert0(str_eq(part_to_str(left), S("Hello World")));
    assert0(str_eq(part_to_str(right), S0));
    assert0(part_join(left, right));
    assert0(str_eq(part_to_str(left), S("Hello World")));
    assert0(str_eq(part_to_str(right), S0));
    part_free(mem, right);

    part_delete(left, 0, 6);
    assert0(str_eq(part_to_str(left), S("World")));
    part_delete(left, 2, 2);
    assert0(str_eq(part_to_str(left), S("Wod")));
    part_delete(left, 2, 1);
    assert0(str_eq(part_to_str(left), S("Wo")));
    part_delete(left, 0, 2);
    assert0(str_eq(part_to_str(left), S0));

    mem_free(mem);
}
