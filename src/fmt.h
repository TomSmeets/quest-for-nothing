// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// fmt.h - a simple sprintf implementation
#pragma once
#include "mem.h"
#include "std.h"
#include "str.h"
#include "types.h"

#include <stdarg.h>

typedef struct {
    u8 *start;
    u32 size;
    u32 used;
} Buffer;

typedef struct {
    // Standard
    bool flag_alt;   // '#' Alternative form
    bool flag_zero;  // '0' Zero padding
    bool flag_left;  // '-' Left Adjusted
    bool flag_space; // ' ' A space when number is positive (for alignment)
    bool flag_group; // ''' Group thousands (9,123,456.0)

    // Custom
    bool flag_si;  // '$' Convert to si  sizes (1K = 1000)
    bool flag_iec; // '/' Convert to iec sizes (1K = 1024)

    u32 width;
    u32 precision;
    char sign_char;

    // Conversion
    u8 conv_base;

    // Output buffer
    Buffer buf;
} Format;

static void fmt_char(Buffer *buf, u8 c) {
    if (buf->used < buf->size) buf->start[buf->used++] = c;
}

static void fmt_bytes(Buffer *buf, u8 *data, u32 size) {
    u32 remaining = buf->size - buf->used;
    if (size > remaining) size = remaining;
    std_memcpy(buf->start + buf->used, data, size);
    buf->used += size;
}

static void fmt_repeat(Buffer *buf, u8 data, u32 size) {
    u32 remaining = buf->size - buf->used;
    if (size > remaining) size = remaining;
    std_memset(buf->start + buf->used, data, size);
    buf->used += size;
}

static void fmt_str(Format *opt, Buffer *buf, char *str, u32 len) {
    u32 pad = 0;
    if (opt->width > len) pad = opt->width - len;

    if (pad && opt->flag_left) {
        fmt_repeat(buf, ' ', pad);
    }

    fmt_bytes(buf, (u8 *)str, len);

    if (pad && !opt->flag_left) {
        fmt_repeat(buf, ' ', pad);
    }
}

static void fmt_u64(Format *opt, Buffer *buf, u64 value) {
    u32 digit_count = 0;
    u8 digits[64 + 2 + 1];

    assert(opt->conv_base >= 2 && opt->conv_base <= 16, "Invalid Base");

    for (;;) {
        u64 d = value % opt->conv_base;
        value = value / opt->conv_base;
        digits[digit_count++] = d < 10 ? (d + '0') : (d - 10 + 'a');
        if (value == 0) break;
    }

    assert(digit_count > 0, "No digits?");
    assert(digit_count < 64, "Too many digits");

    // Zero padding
    if (opt->flag_zero) {
        while (digit_count < opt->width) {
            digits[digit_count++] = '0';
        }
    }

    if (opt->sign_char) {
        digits[digit_count++] = opt->sign_char;
    }

    u32 pad = 0;
    if (opt->width > digit_count) pad = opt->width - digit_count;

    // Right Pad
    if (!opt->flag_left && pad) fmt_repeat(buf, ' ', pad);

    // Write digits in reverse
    while (digit_count > 0) {
        fmt_char(buf, digits[--digit_count]);
    }

    // Left Pad
    if (opt->flag_left && pad) fmt_repeat(buf, ' ', pad);
}

static void fmt_i64(Format *opt, Buffer *buf, i64 value) {
    if (value < 0) {
        opt->sign_char = '-';
        value = -value;
    }

    fmt_u64(opt, buf, (u64)value);
}

static void fmt_f64(Format *opt, Buffer *buf, f64 value) {
    // Truncate towrads 0
    i64 i_part = (i64)value;

    // value = f_part
    value -= i_part;

    // Remove sign
    if (value < 0) value = -value;

    u32 width = opt->width;
    u32 f_width = opt->precision;
    u32 i_width = 0;

    if (width > f_width + 1) {
        i_width = width - f_width - 1;
    }

    for (u32 i = 0; i < f_width; ++i) {
        value *= 10.0;
    }

    u64 f_part = value;

    opt->width = i_width;
    fmt_i64(opt, buf, i_part);
    fmt_char(buf, '.');
    opt->width = f_width;
    opt->flag_zero = 1;
    opt->sign_char = 0;
    fmt_u64(opt, buf, f_part);
}

static void fmt_zero_terminate(Buffer *buf) {
    if (buf->used == buf->size) {
        // Zero does not fit, so we replace the last char
        buf->start[buf->size - 1] = 0;
    } else {
        // Zero does fit
        buf->start[buf->used++] = 0;
    }
}

static void fmt_buf_va(Buffer *buf, char *format, va_list arg) {
    // 5   0      1         2             3            4
    // %[flags][width][.precision][length modifier]conversion
    char *f = format;
    u32 state = 5;

    Format opt = {};

    for (;;) {
        u8 c = *f++;

        // Done
        if (c == 0) break;

        // Format Start
        if (state == 5 && c == '%') {
            state = 0;
            opt = (Format){};
            opt.conv_base = 10;
            opt.precision = 2;
            continue;
        }

        // Flags
        if (state == 0) {
            // clang-format off
            if (c == '#')  { opt.flag_alt   = 1; continue; }
            if (c == '0')  { opt.flag_zero  = 1; continue; }
            if (c == '-')  { opt.flag_left  = 1; continue; }
            if (c == ' ')  { opt.flag_space = 1; continue; }
            if (c == '+')  { opt.sign_char  = '+'; continue; }
            if (c == '\'') { opt.flag_group = 1; continue; }
            if (c == '$')  { opt.flag_si    = 1; continue; }
            if (c == '/')  { opt.flag_iec   = 1; continue; }
            // clang-format on
        }

        // Width (start)
        if (state == 0 && c >= '1' && c <= '9') {
            state = 1;
            opt.width = c - '0';
            continue;
        }

        // Width (cont)
        if (state == 1 && c >= '0' && c <= '9') {
            opt.width = opt.width * 10 + (c - '0');
            continue;
        }

        // presision (start)
        if (state < 2 && c == '.') {
            state = 2;
            opt.precision = 0;
            continue;
        }

        if (state == 2 && c >= '0' && c <= '9') {
            opt.precision = opt.precision * 10 + (c - '0');
            continue;
        }

        if (state < 4) {
            if (c == 'u') {
                u32 value = va_arg(arg, u32);
                fmt_u64(&opt, buf, value);
                state = 5;
                continue;
            }

            if (c == 'd' || c == 'i') {
                i32 value = va_arg(arg, i32);
                fmt_i64(&opt, buf, value);
                state = 5;
                continue;
            }

            if (c == 'q') {
                u64 value = va_arg(arg, u64);
                fmt_u64(&opt, buf, value);
                state = 5;
                continue;
            }

            if (c == 'Q') {
                i64 value = va_arg(arg, i64);
                fmt_i64(&opt, buf, value);
                state = 5;
                continue;
            }

            if (c == 'x') {
                opt.conv_base = 16;
                u32 value = va_arg(arg, u32);
                fmt_u64(&opt, buf, value);
                state = 5;
                continue;
            }

            if (c == 'o') {
                opt.conv_base = 8;
                u32 value = va_arg(arg, u32);
                fmt_u64(&opt, buf, value);
                state = 5;
                continue;
            }

            if (c == 'b') {
                opt.conv_base = 2;
                u32 value = va_arg(arg, u32);
                fmt_u64(&opt, buf, value);
                state = 5;
                continue;
            }

            if (c == 'f') {
                f64 value = va_arg(arg, f64);
                fmt_f64(&opt, buf, value);
                state = 5;
                continue;
            }

            // Exponent
            // Not supported: e, g, a

            if (c == 'c') {
                char c = va_arg(arg, int);
                fmt_str(&opt, buf, &c, 1);
                state = 5;
                continue;
            }

            if (c == 's') {
                char *s = va_arg(arg, char *);
                fmt_str(&opt, buf, s, str_len(s));
                state = 5;
                continue;
            }
        }

        if (state == 5) {
            fmt_char(buf, c);
            continue;
        }
    }
}

__attribute__((format(printf, 1, 2))) static void os_printf(char *format, ...) {
    OS_Alloc *tmp = os_alloc();

    // Use the entire allocation for this format
    Buffer buf;
    buf.start = (u8 *)tmp;
    buf.size = OS_ALLOC_SIZE;
    buf.used = 0;

    // Format
    va_list arg;
    va_start(arg, format);
    fmt_buf_va(&buf, format, arg);
    va_end(arg);

    // Print
    os_write(buf.start, buf.used);

    // Restore allocation
    tmp->next = 0;
    os_free(tmp);
}

__attribute__((format(printf, 2, 3))) static char *fmt(Memory *mem, char *format, ...) {
    OS_Alloc *tmp = os_alloc();

    // Use the entire allocation for this format
    Buffer buf;
    buf.start = (u8 *)tmp;
    buf.size = OS_ALLOC_SIZE;
    buf.used = 0;

    // Format
    va_list arg;
    va_start(arg, format);
    fmt_buf_va(&buf, format, arg);
    va_end(arg);

    // Zero terminate
    fmt_zero_terminate(&buf);

    // Copy to new memory
    char *dst = mem_push_uninit(mem, buf.used);
    std_memcpy(dst, buf.start, buf.used);

    // Restore allocation
    tmp->next = 0;
    os_free(tmp);

    return dst;
}
