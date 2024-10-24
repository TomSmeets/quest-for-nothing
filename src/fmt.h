// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// fmt.h - A simple text formatter
#pragma once

// This formatter has very little dependiecies
#include "mem.h"
#include "os_api.h"
#include "std.h"
#include "str.h"

#if 1
// Should be initialized by os_init
#define OS_FMT ((Fmt *)OS_GLOBAL->fmt)

// Simple flexible buffered string formatter and printer
typedef struct {
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

// Append a single character
//   If full    -> grow buffer
//   On newline -> flush output
static void fmt_c(Fmt *fmt, u8 chr) {
    // if (fmt->used == sizeof(fmt->data)) {
    //     fmt_flush(fmt);
    // }

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
    for (u32 i = 0; i < size; ++i)
        fmt_c(fmt, data[i]);
}

// Append a null terminated string
static void fmt_s(Fmt *fmt, char *str) {
    while (*str)
        fmt_c(fmt, *str++);
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
    for (u32 i = 0; i < pad; ++i)
        fmt_c(fmt, chr);

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
    // Truncate towards 0
    i32 i_part = (i32)value;

    // value = f_part
    value -= i_part;

    // Remove sign
    if (value < 0) value = -value;

    u32 f_width = 4;
    for (u32 i = 0; i < f_width; ++i) {
        value *= 10.0f;
    }
    u32 f_part = value + 0.5f;

    fmt_i(fmt, i_part);
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

#else

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
    bool flag_vec; // 'v' Vector

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

    if (opt->flag_alt) {
        digits[digit_count++] = 'x';
        digits[digit_count++] = '0';
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
    // Truncate towards 0
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

    u64 f_part = value + 0.5f;

    Format opt2 = *opt;
    opt2.width = i_width;
    fmt_i64(&opt2, buf, i_part);
    fmt_char(buf, '.');
    opt2.width = f_width;
    opt2.flag_zero = 1;
    opt2.sign_char = 0;
    fmt_u64(&opt2, buf, f_part);
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
    // fmt_str(&opt, buf, format, str_len(format));

    for (;;) {
        u8 c = *f++;

        // Done
        if (c == 0) break;

        // Format Start
        if (state == 5 && c == '%') {
            state = 0;
            opt = (Format){};
            opt.conv_base = 10;
            opt.precision = 6;
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
            if (c == 'v')  { opt.flag_vec   = 1; continue; }
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

        // precision (start)
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
                u64 value = va_arg(arg, u64);
                fmt_u64(&opt, buf, value);
                state = 5;
                continue;
            }

            if (c == 'd' || c == 'i') {
                if (opt.flag_vec && opt.width == 2) {
                    v2i value = va_arg(arg, v2i);
                    opt.width = 0;
                    fmt_i64(&opt, buf, value.x);
                    fmt_bytes(buf, (u8 *)", ", 2);
                    fmt_i64(&opt, buf, value.y);
                    state = 5;
                } else {
                    i64 value = va_arg(arg, i64);
                    fmt_i64(&opt, buf, value);
                    state = 5;
                }
                continue;
            }

            if (c == 'x') {
                opt.conv_base = 16;
                u64 value = va_arg(arg, u64);
                fmt_u64(&opt, buf, value);
                state = 5;
                continue;
            }

            if (c == 'p') {
                opt.conv_base = 16;
                opt.flag_alt = true;
                u64 value = va_arg(arg, u64);
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
                if (opt.flag_vec && opt.width == 3) {
                    v3 value = va_arg(arg, v3);
                    opt.width = 0;
                    fmt_f64(&opt, buf, value.x);
                    fmt_bytes(buf, (u8 *)", ", 2);
                    fmt_f64(&opt, buf, value.y);
                    fmt_bytes(buf, (u8 *)", ", 2);
                    fmt_f64(&opt, buf, value.z);
                    state = 5;
                } else if (opt.flag_vec && opt.width == 4) {
                    v4 value = va_arg(arg, v4);
                    opt.width = 4 + opt.precision;
                    fmt_f64(&opt, buf, value.x);
                    fmt_bytes(buf, (u8 *)", ", 2);
                    fmt_f64(&opt, buf, value.y);
                    fmt_bytes(buf, (u8 *)", ", 2);
                    fmt_f64(&opt, buf, value.z);
                    fmt_bytes(buf, (u8 *)", ", 2);
                    fmt_f64(&opt, buf, value.w);
                    state = 5;
                } else {
                    f64 value = va_arg(arg, f64);
                    fmt_f64(&opt, buf, value);
                    state = 5;
                }
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

#define os_printf(...) os_fprintf(os_stdout(), __VA_ARGS__)

__attribute__((format(printf, 2, 3))) static void os_fprintf(File *file, const char *format, ...) {
    // Use the entire allocation for this format
    Buffer buf;
    buf.start = os_alloc();
    buf.size = OS_ALLOC_SIZE;
    buf.used = 0;

    // Format
    va_list arg;
    va_start(arg, format);
    fmt_buf_va(&buf, (char *)format, arg);
    va_end(arg);

    // Print
    os_write(file, buf.start, buf.used);

    // Restore allocation
    os_free(buf.start);
}

__attribute__((format(printf, 2, 3))) static char *fmt(Memory *mem, const char *format, ...) {
    OS_Alloc *tmp = os_alloc();

    // Use the entire allocation for this format
    Buffer buf;
    buf.start = (u8 *)tmp;
    buf.size = OS_ALLOC_SIZE;
    buf.used = 0;

    // Format
    va_list arg;
    va_start(arg, format);
    fmt_buf_va(&buf, (char *)format, arg);
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
#endif
