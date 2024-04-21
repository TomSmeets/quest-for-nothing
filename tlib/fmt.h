// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// fmt.h - a simple sprintf implementation
#pragma once
#include "tlib/inc.h"
#include "tlib/math.h"
#include "tlib/mem.h"
#include "tlib/str.h"
#include <stdarg.h>
#include <stdio.h>

typedef Format fmt_t;
struct Format {
    u8 *buffer;
    u64 size;
    u64 used;
    mem *mem;

    // Config
    u32 pad;
    u32 base;
    u8 sign_char;   // +/- sign character
    u8 prefix_char; // prefix char x -> 0x...,
};

static Format *fmt_new(mem *m) {
    Format *f = mem_struct(m, Format);
    f->mem = m;
    return f;
}

// Grow the internal buffer if needed
static void fmt_ensure_size(Format *f, u64 target_size) {
    // Check if we need to grow
    if (target_size <= f->size)
        return;

    // We will need memory now
    assert(f->mem);

    // Determine a good size
    u64 new_size = 32;
    while (new_size < target_size)
        new_size *= 1.5;

    // Copy data to a new buffer
    u8 *new_buffer = mem_push(f->mem, new_size);
    std_memcpy(new_buffer, f->buffer, f->used);
    assert(new_buffer);

    // Done
    f->size = new_size;
    f->buffer = new_buffer;
}

// Push a buffer
static void fmt_buf(Format *f, u64 size, u8 *data) {
    fmt_ensure_size(f, f->used + size);
    std_memcpy(f->buffer + f->used, data, size);
    f->used += size;
}

// Push a single character
static void fmt_chr(Format *f, u8 c) {
    fmt_ensure_size(f, f->used + 1);
    f->buffer[f->used] = c;
    f->used++;
}

// Push a substring
static void fmt_str_len(Format *f, u32 len, char *str) {
    fmt_buf(f, len, (u8 *)str);

    // padding
    while (f->pad > len) {
        fmt_chr(f, ' ');
        f->pad--;
    }
    f->pad = 0;
}

// Push a null terminated string
static void fmt_str(Format *f, char *str) {
    fmt_str_len(f, str_len(str), str);
}

static char *fmt_end(Format *f) {
    fmt_chr(f, 0);
    char *ret = (char *)f->buffer;

    // Reset the formatter
    // We can still re-use the last part
    // that was not yet used.
    f->buffer += f->used;
    f->size -= f->used;
    f->used = 0;

    return ret;
}

static void fmt_pad(Format *f, u32 pad) {
    f->pad = pad;
}
static void fmt_zero_pad(Format *f, u32 pad) {
    f->pad = pad;
    f->prefix_char = '0';
}
static void fmt_base(Format *f, u32 base) {
    f->base = base;
}

// Fancy integer formatting
static void fmt_u64(Format *f, u64 v) {
    u64 base = f->base ? f->base : 10;
    u64 pad = f->pad;
    u64 prefix_char = f->prefix_char;
    f->base = 0;
    f->pad = 0;
    f->prefix_char = 0;
    assert(base >= 2 && base < 36);
    assert(pad <= 64);

    // Accumulate digits in reverse order
    // Doing this in reverse makes everything simpler
    u32 digit_count = 0;
    u8 digits[64 + 2 + 1]; // max_pad + "0x" + sign

    for (;;) {
        u64 mod = v % base;
        v /= base;
        assert(digit_count < 64);
        digits[digit_count++] = mod < 10 ? (mod + '0') : (mod - 10 + 'a');
        if (v == 0)
            break;
    }

    if (prefix_char) {
        while (digit_count < pad)
            digits[digit_count++] = '0';

        // we have a prefix, so pad with 0
        if (prefix_char != '0') {
            digits[digit_count++] = prefix_char;
            digits[digit_count++] = '0';
        }
    }

    if (f->sign_char) {
        digits[digit_count++] = f->sign_char;
        f->sign_char = 0;
    }

    while (digit_count < pad)
        digits[digit_count++] = ' ';

    assert(digit_count <= sizeof(digits));

    std_reverse(digit_count, digits);
    fmt_buf(f, digit_count, digits);
}

// static void fmt_u64(Format *f, u64 v, u64 base, u64 pad, u8 sign) {
static void fmt_i64(Format *f, i64 v) {
    if (v < 0)
        f->sign_char = '-';
    fmt_u64(f, i_abs(v));
}

static void fmt_f32(Format *f, f32 v) {
    f32 vp = v;
    if (v < 0)
        vp = -v;

    assert(f->base == 0);
    assert(f->sign_char == 0);

    vp += 0.0005;
    i32 i_part = f_trunc(vp);
    f32 f_part = f_fract(vp);
    if (v < 0)
        fmt_chr(f, '-');
    fmt_u64(f, i_part);
    fmt_chr(f, '.');
    fmt_zero_pad(f, 3);
    fmt_u64(f, f_part * 1000);
}

// %xa.b
static void fmt_va(Format *f, char *s, va_list args) {
    bool in_arg = 0;
    u32 sign = 0;
    u32 pad = 0;
    for (;;) {
        char c = *s++;
        if (!c)
            break;

        if (c == '%') {
            in_arg = 1;
            f->pad = 0;
            f->sign_char = 0;
            f->base = 0;
        } else if (in_arg && c == '+') {
            f->sign_char = '+';
        } else if (in_arg && c >= '0' && c <= '9') {
            // %02d
            if (f->pad == 0 && c == '0')
                f->prefix_char = '0';
            f->pad = f->pad * 10 + (c - '0');
        } else if (in_arg && (c == 'i' || c == 'd')) {
            fmt_i64(f, va_arg(args, i32));
            in_arg = 0;
        } else if (in_arg && c == 'u') {
            fmt_u64(f, va_arg(args, u64));
            in_arg = 0;
        } else if (in_arg && c == 'x') {
            f->base = 16;
            f->prefix_char = 'x';
            fmt_u64(f, va_arg(args, u64));
            in_arg = 0;
        } else if (in_arg && c == 's') {
            fmt_str(f, va_arg(args, char *));
            in_arg = 0;
        } else if (in_arg && c == 'S') {
            char *start = va_arg(args, char *);
            char *end = va_arg(args, char *);
            fmt_str_len(f, end - start, start);
            in_arg = 0;
        } else if (in_arg && c == 'c') {
            fmt_chr(f, va_arg(args, u32));
            in_arg = 0;
        } else if (in_arg && c == 'f') {
            fmt_f32(f, (f32)va_arg(args, double));
            in_arg = 0;
        } else if (!in_arg) {
            fmt_chr(f, c);
        } else {
            assert(!"Unknown format");
        }
    }
}

static void fmt_f(Format *f, char *format, ...) {
    va_list args;
    va_start(args, format);
    fmt_va(f, format, args);
    va_end(args);
}

__attribute__((format(printf, 2, 3))) static char *fmt(mem *m, char *format, ...) {
    va_list args;
    Format f = {.mem = m};
    va_start(args, format);
    fmt_va(&f, format, args);
    va_end(args);
    return fmt_end(&f);
}

__attribute__((format(printf, 1, 2))) static void os_printf(char *format, ...) {
    va_list args;

    mem m = {};
    Format f = {.mem = &m};
    va_start(args, format);
    fmt_va(&f, format, args);
    va_end(args);
    os_print(fmt_end(&f));
    mem_clear(&m);
}

static u32 fmt_pad_start(Format *f) {
    return f->used;
}

// pad:   number of pad chars
// start: result from a fmt_pad_start
static void fmt_rpad(Format *f, u32 start, u32 pad) {
    while (start + pad > f->used)
        fmt_str(f, " ");
}

static void fmt_lpad(Format *f, u32 start, u32 pad) {
    // start              fmt_end
    // v                    v
    // | data | . .  .  . . |
    //
    //      converted to
    //
    // | . . . . . . | data |

    u32 data_end = f->used;
    u32 fmt_end = start + pad;
    u32 data_size = data_end - start;

    if (data_end >= fmt_end)
        return;

    // fill extra space with spaces
    while (fmt_end > f->used)
        fmt_str(f, " ");

    // now fmt_end == end of format
    for (u32 i = 0; i < data_size; ++i) {
        u8 *src = &f->buffer[data_end - i - 1];
        u8 *dst = &f->buffer[fmt_end - i - 1];
        *dst = *src;
        *src = ' ';
    }
}
