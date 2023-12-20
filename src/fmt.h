// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// fmt.h - a simple sprintf implementation
#pragma once
#include "inc.h"
#include "math.h"
#include "mem.h"
#include "str.h"
#include <stdarg.h>
#include <stdio.h>

// Note we can look into the future, create a fmt with empty buffer
typedef struct fmt_t fmt_t;
struct fmt_t {
    u8 *buffer;
    u64 size;
    u64 used;
};

static void fmt_buf(fmt_t *f, u64 size, u8 *data) {
    if (f->used + size <= f->size)
        std_memcpy(f->buffer + f->used, data, size);
    f->used += size;
}

static void fmt_str_len(fmt_t *f, u32 len, char *str) {
    fmt_buf(f, len, (u8 *)str);
}

// Push a string onto this buffer
static void fmt_str(fmt_t *f, char *str) {
    fmt_str_len(f, str_len(str), str);
}

static void fmt_chr(fmt_t *f, u8 c) {
    if (f->used < f->size)
        f->buffer[f->used] = c;
    f->used++;
}

static char *fmt_end(fmt_t *f) {
    if (f->used >= f->size)
        return 0;

    f->buffer[f->used] = 0;
    return (char *)f->buffer;
}

//
static void fmt_u64(fmt_t *f, u64 v, u64 base, u64 pad, u8 prefix, u8 sign) {
    assert(base >= 2 && base < 36);
    assert(pad <= 64);

    // compute the prefix character (the 'x' in '0x000')
    // null means no prefix
    if (!prefix && base == 2)  prefix = 'b';
    if (!prefix && base == 8)  prefix = 'o';
    if (!prefix && base == 16) prefix = 'x';

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

    if (prefix) {
        // we have a prefix, so pad with 0
        while (digit_count < pad)
            digits[digit_count++] = '0';

        digits[digit_count++] = prefix;

        if (prefix != '.') digits[digit_count++] = '0';
    }

    if (sign) digits[digit_count++] = sign;

    while (digit_count < pad)
        digits[digit_count++] = ' ';

    assert(digit_count <= sizeof(digits));

    std_reverse(digit_count, digits);
    fmt_buf(f, digit_count, digits);
}

// static void fmt_u64(fmt_t *f, u64 v, u64 base, u64 pad, u8 sign) {
static void fmt_i64(fmt_t *f, i64 v, u64 base, u64 pad, u8 prefix, u8 sign) {
    fmt_u64(f, v < 0 ? -v : v, base, pad, prefix, v < 0 ? '-' : sign);
}

static void fmt_f32(fmt_t *f, f32 v, u64 pad, u8 prefix, u8 sign) {
    v += 0.0005;
    i32 i_part = f_trunc(v);
    f32 f_part = f_fract(v);
    fmt_i64(f, i_part, 10, pad, prefix, 0);
    fmt_i64(f, f_part * 1000, 10, 3, '.', 0);
}

// %xa.b
static void fmt_va(fmt_t *f, char *s, va_list args) {
    bool in_arg = 0;
    bool sign = 0;
    u32 pad = 0;
    for (;;) {
        char c = *s++;
        if (!c) break;

        if (c == '%') {
            in_arg = 1;
            pad = 0;
            sign = 0;
        } else if (in_arg && c == '+') {
            sign = '+';
        } else if (in_arg && c >= '0' && c <= '9') {
            pad = pad * 10 + (c - '0');
        } else if (in_arg && (c == 'i' || c == 'd')) {
            fmt_i64(f, va_arg(args, i32), 10, pad, 0, sign);
            in_arg = 0;
        } else if (in_arg && c == 'u') {
            fmt_u64(f, va_arg(args, u64), 10, pad, 0, sign);
            in_arg = 0;
        } else if (in_arg && c == 'x') {
            fmt_u64(f, va_arg(args, u64), 16, pad, 0, sign);
            in_arg = 0;
        } else if (in_arg && c == 's') {
            fmt_str(f, va_arg(args, char *));
            in_arg = 0;
        } else if (in_arg && c == 'S') {
            char *start = va_arg(args, char *);
            char *end   = va_arg(args, char *);
            fmt_str_len(f, end - start, start);
            in_arg = 0;
        } else if (in_arg && c == 'c') {
            fmt_chr(f, va_arg(args, u32));
            in_arg = 0;
        } else if (in_arg && c == 'f') {
            fmt_f32(f, (f32)va_arg(args, double), pad, 0, sign);
            in_arg = 0;
        } else if (!in_arg) {
            fmt_chr(f, c);
        } else {
            assert(!"Unknown format");
        }
    }
}

static void fmt_f(fmt_t *f, char *format, ...) {
    va_list args;
    va_start(args, format);
    fmt_va(f, format, args);
    va_end(args);
}

static char *fmt(mem *m, char *format, ...) {
    va_list args;

    fmt_t f = {};
    va_start(args, format);
    fmt_va(&f, format, args);
    va_end(args);

    f.size = f.used + 1;
    f.buffer = mem_push(m, f.size);
    f.used = 0;

    va_start(args, format);
    fmt_va(&f, format, args);
    va_end(args);

    char *ret = fmt_end(&f);
    assert(ret);
    return ret;
}

static void os_printf(char *format, ...) {
    va_list args;

    fmt_t f = {};
    va_start(args, format);
    fmt_va(&f, format, args);
    va_end(args);

    mem m = {};
    f.size = f.used + 1;
    f.buffer = mem_push(&m, f.size);
    f.used = 0;

    va_start(args, format);
    fmt_va(&f, format, args);
    va_end(args);

    char *ret = fmt_end(&f);
    assert(ret);
    os_print(ret);
    mem_clear(&m);
}
