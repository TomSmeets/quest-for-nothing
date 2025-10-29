// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// text.h - Text buffer
#pragma once
#include "lib/fmt.h"
#include "lib/mem.h"
#include "lib/os_alloc.h"
#include "lib/part.h"
#include "lib/str_mem.h"
#include "lib/test.h"
#include "lib/types.h"

TYPEDEF_STRUCT(Text);

struct Text {
    Memory *mem;
    Part *part;
};

static Text *text_new(Memory *mem) {
    Text *text = mem_struct(mem, Text);
    text->mem = mem;
    return text;
}

static void text_append(Text *text, String str) {
    if (str.len == 0) return;
    if (!text->part) text->part = part_new(text->mem);

    Part *last = text->part;
    while (last->next) last = last->next;

    for (;;) {
        u32 added = part_fill(last, str).len;
        str.data += added;
        str.len -= added;
        if (str.len == 0) break;
        Part *next = part_new(text->mem);
        last->next = next;
        last = next;
    }
}

// Return right part
static Text text_split(Text *text, u32 pos) {
    Text ret = {};
    ret.mem = text->mem;

    Part *left = text->part;
    while (pos > left->len) {
        pos -= left->len;
        left = left->next;
    }

    // Split
    if (pos == left->len) {
        ret.part = left->next;
        left->next = 0;
    } else {
        Part *right = part_split(text->mem, left, pos);
        right->next = left->next;
        left->next = 0;
        ret.part = right;
    }
    return ret;
}

static void text_join(Text *text, Text *right) {
    if (!text->part) {
        text->part = right->part;
        return;
    }

    if (!right->part) {
        return;
    }

    // Find last part in 'text'
    Part *left_part = text->part;
    while (left_part->next) left_part = left_part->next;

    // Join into left
    Part *right_part = right->part;

    // Join parts
    if (part_join(left_part, right_part)) {
        Part *next = right_part->next;
        part_free(text->mem, right_part);
        right_part = next;
    }

    left_part->next = right_part;
    right->part = 0;
}

static void text_free(Text *text) {
    Part *part = text->part;
    while (part) {
        Part *next = part->next;
        part_free(text->mem, part);
        part = next;
    }
}

static void text_insert(Text *text, u32 pos, String str) {
    if (str.len == 0) return;
    Text right = text_split(text, pos);
    text_append(text, str);
    text_join(text, &right);
}

static void text_delete(Text *text, u32 pos, u32 size) {
    Text mid = text_split(text, pos);
    Text right = text_split(&mid, size);
    text_join(text, &right);
    text_free(&mid);
}

static Text *text_from(Memory *mem, String str) {
    Text *text = text_new(mem);
    text_append(text, str);
    return text;
}

static bool text_eq(Text *text, String str) {
    // For every piece check that the string starts with it
    // then remove that piece from the string until no piecers are left
    for (Part *part = text->part; part; part = part->next) {
        if (!str_drop_start_matching(&str, part_to_str(part))) return false;
    }

    // String was longer than the text
    if (str.len > 0) return false;
    return true;
}

static void text_debug(Text *text, Fmt *fmt) {
    fmt_s(fmt, "Text:\n");
    u32 index = 0;
    for (Part *part = text->part; part; part = part->next) {
        fmt_suu(fmt, "=== Part Index ", index++, " len=", part->len, " ===\n");
        fmt_buf(fmt, (u8 *)part->data, part->len);
        fmt_s(fmt, "\n");
    }
}

static void text_fmt(Text *text, Fmt *fmt) {
    for (Part *part = text->part; part; part = part->next) {
        fmt_buf(fmt, (u8 *)part->data, part->len);
    }
}

static u32 text_len(Text *text) {
    u32 len = 0;
    Part *part = text->part;
    while (part) {
        len += part->len;
        part = part->next;
    }
    return len;
}

static u8 text_index(Text *text, u32 pos) {
    Part *part = text->part;
    while (pos >= part->len) {
        pos -= part->len;
        part = part->next;
    }
    return part->data[pos];
}

// Slow, but works
static bool text_find(Text *text, u32 start, String key, u32 *result) {
    u32 len = text_len(text);
    if (key.len > len) return false;
    if (key.len == 0) {
        *result = start;
        return true;
    }
    for (u32 i = start; i < len - key.len + 1; ++i) {
        bool matches = true;
        for (u32 j = 0; j < key.len; ++j) {
            u8 left = text_index(text, i + j);
            u8 right = key.data[j];
            if (left != right) {
                matches = false;
                break;
            }
        }
        if (matches) {
            *result = i;
            return true;
        }
    }
    return false;
}

// TODO: Text
static void text_test(Test *test) {
    Memory *mem = test->mem;
    Fmt *fmt = test->fmt;

    Text *text = text_from(mem, S0);
    TEST(text->mem == mem);
    TEST(text->part == 0);

    // Join
    Text *text0 = text_from(mem, S("Hello"));
    Text *text1 = text_from(mem, S(" "));
    Text *text2 = text_from(mem, S("World"));
    text_join(text0, text1);
    text_join(text0, text2);
    text_debug(text0, fmt);
    TEST(text_eq(text0, S("Hello World")));

    Text text3 = text_split(text0, 6);
    TEST(text_eq(text0, S("Hello ")));
    TEST(text_eq(&text3, S("World")));

    // Append
    String hello = S("Hello");
    String world = S(" World!");
    text_append(text, hello);
    text_debug(text, fmt);
    TEST(text_eq(text, S("Hello!")) == 0); // (too long)
    TEST(text_eq(text, S("Hello")) == 1);  // OK!

    text_append(text, world);
    text_debug(text, fmt);
    TEST(text_eq(text, S("Hello World!")));

    String awesome = S("AWESOME ");
    text_insert(text, hello.len + 1, awesome);
    text_debug(text, fmt);
    TEST(text_eq(text, S("Hello AWESOME World!")));

    u32 pos = 0;
    TEST(text_find(text, 0, S("Hello"), &pos));
    TEST(pos == 0);
    TEST(text_find(text, 0, S("AWESOME"), &pos));
    TEST(pos == 6);
    TEST(text_find(text, 0, S("World!"), &pos));
    TEST(pos == 14);
    TEST(text_find(text, 0, S(" "), &pos));
    TEST(pos == 5);
    TEST(text_find(text, 5, S(" "), &pos));
    TEST(pos == 5);
    TEST(text_find(text, 6, S(" "), &pos));
    TEST(pos == 13);
    TEST(text_find(text, 14, S(" "), &pos) == false);

    text_append(text, S("\n"));
    for (u32 i = 0; i < 26 * 2; ++i) {
        char c = ('A' + (i % 26));
        text_append(text, S("Hello World: "));
        text_append(text, (String){1, (u8 *)&c});
        text_append(text, S("\n"));
    }
    text_debug(text, fmt);
    text_append(text, S("\n"));

    u32 len = 15;
    u32 cursor = 21;
    fmt_s(fmt, "\n=== Add Numbers ===\n");
    for (u32 i = 0; i < 26 * 2; ++i) {
        text_insert(text, cursor++, S("0"));
        text_insert(text, cursor++, S("1"));
        text_insert(text, cursor++, S("2"));
        text_insert(text, cursor++, S("3"));
        cursor += len;
    }
    text_fmt(text, fmt);

    fmt_s(fmt, "\n=== DELETE Numbers ===\n");
    cursor = 21;
    for (u32 i = 0; i < 26 * 2; ++i) {
        text_delete(text, cursor, 4);
        cursor += len;
    }
    text_fmt(text, fmt);

    fmt_s(fmt, "\n=== Replace world -> WORLD ===\n");
    String rem = S(" World");
    while (text_find(text, 0, rem, &pos)) {
        text_delete(text, pos, rem.len);
    }
    text_fmt(text, fmt);

    fmt_s(fmt, "\n=== DELETE ALL ===\n");
    text_delete(text, 0, text_len(text));
    text_fmt(text, G->fmt);
}
