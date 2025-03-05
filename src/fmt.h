// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// fmt.h - A simple text formatter
#pragma once

// This formatter has very little dependiecies
#include "mem.h"
#include "os_desktop.h"
#include "std.h"

// Simple flexible buffered string formatter and printer
typedef struct Fmt {
    // Optional output file (use fmt_open/fmt_close)
    File *out;

    // Number of bytes used in this buffer
    u32 used;
    u8 data[1024];
} Fmt;

// Create a new formatter that optionally writes to a file
static Fmt *fmt_new(Memory *mem, File *out) {
    Fmt *fmt = mem_struct(mem, Fmt);
    fmt->out = out;
    return fmt;
}

// Create a new formatter that opens and writes to a file
static Fmt *fmt_open(Memory *mem, char *path) {
    return fmt_new(mem, os_open(path, Open_Write));
}

// Create a new formatter that only writes to memory
static Fmt *fmt_memory(Memory *mem) {
    return fmt_new(mem, 0);
}

// Write all buffered data to the output stream (if avaliable)
static void fmt_flush(Fmt *fmt) {
    if (!fmt->out) return;
    os_write(fmt->out, fmt->data, fmt->used);
    fmt->used = 0;
}

static void fmt_grow(Fmt *fmt) {
    // Not implemented yet, do we want this?
}

// Append a single character
//   Full    -> grow
//   Newline -> flush
static void fmt_c(Fmt *fmt, u8 chr) {
    if (fmt->used == sizeof(fmt->data)) {
        fmt_grow(fmt);
    }

    assert(fmt->used < sizeof(fmt->data), "Out of memory for this formatter");

    fmt->data[fmt->used++] = chr;

    // Flush on newline
    if (chr == '\n') fmt_flush(fmt);
}

// Close the formatter and the output stream (if it exists)
// Returns the formatted output if there was no output stream
static char *fmt_close(Fmt *fmt) {
    if (fmt->out) {
        fmt_flush(fmt);
        os_close(fmt->out);
        return 0;
    } else {
        fmt_c(fmt, 0);
        return (char *)fmt->data;
    }
}

// Append multiple bytes
static void fmt_buf(Fmt *fmt, u8 *data, u32 size) {
    for (u32 i = 0; i < size; ++i) fmt_c(fmt, data[i]);
}

// Append a null terminated string
static void fmt_s(Fmt *fmt, char *str) {
    while (*str) fmt_c(fmt, *str++);
}

// Return current location
// Used for padding and range operations
static u32 fmt_cursor(Fmt *fmt) {
    return fmt->used;
}

// Reverse chars between cursor and end of the buffer
static void fmt_reverse(Fmt *fmt, u32 cursor) {
    assert(cursor <= fmt->used, "Invalid cursor");
    std_reverse(fmt->data + cursor, fmt->used - cursor);
}

// Add padding to the content starting at 'cursor'
// use 'chr' as the padding character
static void fmt_pad(Fmt *fmt, u32 cursor, u8 chr, u32 pad_total, bool pad_left) {
    assert(cursor <= fmt->used, "Invalid cursor");
    u32 len = fmt->used - cursor;

    // Text is longer than requested padding
    if (pad_total <= len) return;

    u32 pad = pad_total - len;

    // Create space
    for (u32 i = 0; i < pad; ++i) {
        fmt_c(fmt, chr);
    }

    assert(fmt->used - cursor == pad_total, "Incorreclty calculated padding");

    if (pad_left) {
        // Move data left
        for (u32 i = 0; i < len; ++i) {
            // cursor    used
            //    | ABCD |
            //    | ...ABCD |
            //
            fmt->data[cursor + pad + i] = fmt->data[cursor + i];
            fmt->data[cursor + i] = chr;
        }
    }
}

// Format an unsigned integer
static void fmt_u_base(Fmt *fmt, u32 base, u64 value) {
    u32 fmt_start = fmt_cursor(fmt);

    // Push digits in reverse order
    for (;;) {
        u64 d = value % base;
        value = value / base;
        fmt_c(fmt, d < 10 ? (d + '0') : (d - 10 + 'a'));
        if (value == 0) break;
    }

    // Reverse digits
    fmt_reverse(fmt, fmt_start);
}

static void fmt_u(Fmt *fmt, u32 value) {
    fmt_u_base(fmt, 10, value);
}

// Format a signed integer
static void fmt_i(Fmt *fmt, i32 value) {
    if (value < 0) {
        fmt_c(fmt, '-');
        fmt_u(fmt, -value);
    } else {
        fmt_u(fmt, value);
    }
}

static void fmt_x(Fmt *fmt, u32 value) {
    fmt_s(fmt, "0x");
    u32 pad_start = fmt_cursor(fmt);
    fmt_u_base(fmt, 16, value);
    fmt_pad(fmt, pad_start, '0', 8, true);
}

static void fmt_p(Fmt *fmt, void *ptr) {
    fmt_s(fmt, "0x");
    u32 pad_start = fmt_cursor(fmt);
    fmt_u_base(fmt, 16, (u64)ptr);
    fmt_pad(fmt, pad_start, '0', 16, true);
}

static void fmt_f(Fmt *fmt, f32 value) {
    bool sign = false;

    // Remove sign
    if (value < 0) {
        sign = true;
        value = -value;
    }

    // Truncate towards 0
    u32 i_part = (u32)value;
    value -= i_part;

    u32 f_width = 4;
    for (u32 i = 0; i < f_width; ++i) {
        value *= 10.0f;
    }
    u32 f_part = value + 0.5f;

    if (sign) fmt_c(fmt, '-');
    fmt_u(fmt, i_part);
    fmt_c(fmt, '.');

    u32 cur = fmt_cursor(fmt);
    fmt_u(fmt, f_part);
    fmt_pad(fmt, cur, '0', f_width, true);
}

static void fmt_ss(Fmt *fmt, char *a0, char *a1, char *a2) {
    fmt_s(fmt, a0);
    fmt_s(fmt, a1);
    fmt_s(fmt, a2);
}

static void fmt_su(Fmt *fmt, char *a0, u32 a1, char *a2) {
    fmt_s(fmt, a0);
    fmt_u(fmt, a1);
    fmt_s(fmt, a2);
}

static void fmt_si(Fmt *fmt, char *a0, i32 a1, char *a2) {
    fmt_s(fmt, a0);
    fmt_i(fmt, a1);
    fmt_s(fmt, a2);
}

static void fmt_sx(Fmt *fmt, char *a0, u32 a1, char *a2) {
    fmt_s(fmt, a0);
    fmt_x(fmt, a1);
    fmt_s(fmt, a2);
}

static void fmt_sf(Fmt *fmt, char *a0, f32 a1, char *a2) {
    fmt_s(fmt, a0);
    fmt_f(fmt, a1);
    fmt_s(fmt, a2);
}

static void fmt_sss(Fmt *fmt, char *a0, char *a1, char *a2, char *a3, char *a4) {
    fmt_s(fmt, a0);
    fmt_s(fmt, a1);
    fmt_s(fmt, a2);
    fmt_s(fmt, a3);
    fmt_s(fmt, a4);
}

static void fmt_ssss(Fmt *fmt, char *a0, char *a1, char *a2, char *a3, char *a4, char *a5, char *a6) {
    fmt_s(fmt, a0);
    fmt_s(fmt, a1);
    fmt_s(fmt, a2);
    fmt_s(fmt, a3);
    fmt_s(fmt, a4);
    fmt_s(fmt, a5);
    fmt_s(fmt, a6);
}

static void fmt_suu(Fmt *fmt, char *a0, u32 a1, char *a2, u32 a3, char *a4) {
    fmt_s(fmt, a0);
    fmt_u(fmt, a1);
    fmt_s(fmt, a2);
    fmt_u(fmt, a3);
    fmt_s(fmt, a4);
}

static void fmt_suuu(Fmt *fmt, char *a0, u32 a1, char *a2, u32 a3, char *a4, u32 a5, char *a6) {
    fmt_s(fmt, a0);
    fmt_u(fmt, a1);
    fmt_s(fmt, a2);
    fmt_u(fmt, a3);
    fmt_s(fmt, a4);
    fmt_u(fmt, a5);
    fmt_s(fmt, a6);
}

static void fmt_siii(Fmt *fmt, char *a0, i32 a1, char *a2, i32 a3, char *a4, i32 a5, char *a6) {
    fmt_s(fmt, a0);
    fmt_i(fmt, a1);
    fmt_s(fmt, a2);
    fmt_i(fmt, a3);
    fmt_s(fmt, a4);
    fmt_i(fmt, a5);
    fmt_s(fmt, a6);
}
static void fmt_sff(Fmt *fmt, char *a0, f32 a1, char *a2, f32 a3, char *a4) {
    fmt_s(fmt, a0);
    fmt_f(fmt, a1);
    fmt_s(fmt, a2);
    fmt_f(fmt, a3);
    fmt_s(fmt, a4);
}

static void fmt_sfff(Fmt *fmt, char *a0, f32 a1, char *a2, f32 a3, char *a4, f32 a5, char *a6) {
    fmt_s(fmt, a0);
    fmt_f(fmt, a1);
    fmt_s(fmt, a2);
    fmt_f(fmt, a3);
    fmt_s(fmt, a4);
    fmt_f(fmt, a5);
    fmt_s(fmt, a6);
}

static void fmt_sffff(Fmt *fmt, char *a0, f32 a1, char *a2, f32 a3, char *a4, f32 a5, char *a6, f32 a7, char *a8) {
    fmt_s(fmt, a0);
    fmt_f(fmt, a1);
    fmt_s(fmt, a2);
    fmt_f(fmt, a3);
    fmt_s(fmt, a4);
    fmt_f(fmt, a5);
    fmt_s(fmt, a6);
    fmt_f(fmt, a7);
    fmt_s(fmt, a8);
}
