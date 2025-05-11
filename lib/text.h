#pragma once
#include "fmt.h"
#include "mem.h"
#include "str_mem.h"
#include "types.h"

// A "Rope" is just a linked list of Strings
// NOTE: this is not (yet) implemented as a rope datastructure
// TODO: implmeent skiplist, this seems like a good structure for this
typedef struct Rope Rope;
struct Rope {
    Rope *next;
    u8 len;
    u8 data[255];
};

// Create a new and empty rope node
static Rope *rope_new(Memory *mem) {
    Rope *rope = mem_struct_uninit(mem, Rope);
    rope->next = 0;
    rope->len = 0;
    return rope;
}

// Consume data from 'str' and put into rope
// the string is consumed until this rope instance is full
static void rope_fill_one(Rope *rope, String *str) {
    u32 len = str->len;
    u32 size_left = array_count(rope->data) - rope->len;
    if (len > size_left) len = size_left;

    std_memcpy(rope->data + rope->len, str->data, len);
    rope->len += len;

    // Reduce string size
    str->data += len;
    str->len -= len;
}

// Append entire string
static void rope_append(Memory *mem, Rope **node, String str) {
    for (;;) {
        rope_fill_one(*node, &str);
        if (str.len == 0) break;

        Rope *next = rope_new(mem);
        next->next = (*node)->next;
        (*node)->next = next;
        *node = next;
    }
}

static String rope_to_str(Rope *rope) {
    return (String) { rope->len, rope->data };
}

typedef struct Text Text;
struct Text {
    Memory *mem;
    Rope *first;
    Rope *last;
    u32 cursor;
};

static Text *text_new(Memory *mem) {
    Text *text = mem_struct(mem, Text);
    text->mem = mem;
    text->first = text->last = rope_new(mem);
    return text;
}

// Append entire string
static void text_append(Text *text, String str) {
    rope_append(text->mem, &text->last, str);
}

static Text *text_from(Memory *mem, String str) {
    Text *text = text_new(mem);
    text_append(text, str);
    return text;
}

static bool text_eq(Text *text, String str) {
    // For every piece check that the string starts with it
    // then remove that piece from the string until no piecers are left
    for (Rope *rope = text->first; rope; rope = rope->next) {
        if(!str_drop_start_matching(&str, rope_to_str(rope))) return false;
    }

    // String was longer than the text
    if(str.len > 0) return false;

    return true;
}

static void text_insert(Text *text, u32 text_offset, String str) {
    // Seek
    Rope *rope = text->first;
    u32 rope_offset = text_offset;
    while (rope_offset > rope->len) {
        rope_offset -= rope->len;
        rope = rope->next;
    }

    // Split
    u32 left_len = rope_offset;
    u32 right_len = rope->len - left_len;

    // Remove right part of this rope
    u8 temp[sizeof(rope->data)];
    String right_scrach = {
        .len = right_len,
        .data = temp,
    };
    std_memcpy(right_scrach.data, rope->data + left_len, right_scrach.len);
    rope->len = left_len;

    // Append string
    rope_append(text->mem, &rope, str);

    // Append rest
    rope_append(text->mem, &rope, right_scrach);

    // Update last
    if (!rope->next) text->last = rope;
}

static void text_debug(Text *text, Fmt *fmt) {
    fmt_s(fmt, "Text:\n");
    u32 index = 0;
    for (Rope *rope = text->first; rope; rope = rope->next) {
        fmt_su(fmt, "=== Rope Index ", index++, " ===\n");
        fmt_buf(fmt, (u8 *)rope->data, rope->len);
        fmt_s(fmt, "\n");
    }
}

// TODO: Text
static void test_text(void) {
    Memory *mem = mem_new();


    Text *text = text_from(mem, S0);
    assert0(text->mem == mem);
    assert0(text->first != 0);
    assert0(text->first == text->last);
    assert0(text->first->next == 0);
    assert0(text->first->len == 0);

    // Append
    String hello = S("Hello");
    String world = S(" World!");

    text_append(text, hello);
    text_debug(text, G->fmt);
    assert0(text_eq(text, S("Hello!")) == 0); // (too long)
    assert0(text_eq(text, S("Hello")) == 1); // OK!

    text_append(text, world);
    text_debug(text, G->fmt);
    assert0(text_eq(text, S("Hello World!")));

    String awesome = S("AWESOME ");
    text_insert(text, hello.len + 1, awesome);
    text_debug(text, G->fmt);
    assert0(text_eq(text, S("Hello AWESOME World!")));

    text_append(text, S("\n"));
    for(u32 i = 0; i < 26 * 2; ++i) {
        char c = ('A' + (i % 26));
        text_append(text, S("Hello World: "));
        text_append(text, (String){1, (u8*) &c});
        text_append(text, S("\n"));
    }
    text_debug(text, G->fmt);


    text_append(text, S("\n"));

    // TODO: FIX!
    u32 len = 15;
    u32 cursor = 21;
    for(u32 i = 0; i < 26 * 2; ++i) {
        text_insert(text, cursor, S(">>>>"));
        cursor += 4;
        cursor += len;
    }
    text_debug(text, G->fmt);

    mem_free(mem);
}

#if 0
typedef struct {
    Memory *mem;
    String str;
    u32 cursor;
} Text;

// Compress text representation and free unused memory
static Text text_clone(Memory *mem, Text *txt) {
    Text txt2 = *txt;
    txt2.mem = mem;
    txt2.str = str_clone(mem, txt2.str);
    return txt2;
}

static void text_replace(Text *txt, u32 pos, u32 del, String new) {
    txt->str = str_replace(txt->mem, txt->str, pos, del, new);

    // xx___yyy
    // xxnyyy
    // pos: 2
    // del: 3
    if (txt->cursor > pos + del) {
        // After deleted region
        txt->cursor -= del;
        txt->cursor += new.len;
    } else if (txt->cursor > pos) {
        // In deleted region
        txt->cursor = pos;
    }
}

static void test_text(void) {
    Memory *mem = mem_new();
    Text text = {
        .mem = mem,
        .str = S("Hello World!"),
        .cursor = 0,
    };

    text.cursor = 6;
    assert1(text.str.data[text.cursor] == 'W');
    text_replace(&text, 1, 4, S("ii"));
    assert1(string_eq(text.str, S("Hii World!")));
    assert1(text.cursor == 4);
    assert1(text.str.data[text.cursor] == 'W');
    mem_free(mem);
}
#endif
