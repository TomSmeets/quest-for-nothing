#pragma once
#include "fmt.h"
#include "mem.h"
#include "str_mem.h"
#include "os_alloc.h"
#include "types.h"

// A "Rope" is just a linked list of Strings
// NOTE: this is not (yet) implemented as a rope datastructure
// TODO: implmeent skiplist, this seems like a good structure for this
//
// Always N/2 - N data items
// insert c -> add if fits, other wise split, then add
// 
typedef struct Part Part;
struct Part {
    Part *next;
    Part *prev;
    u8 len;
    u8 data[255];
};

// Create a new and empty rope node
static Part *part_new(Memory *mem) {
    Part *part = mem_struct_uninit(mem, Part);
    part->next = 0;
    part->len = 0;
    return part;
}

// Remove part from list and free it
// Pass the next pointer of the previous part
static void part_free(Memory *mem, Part *part) {
    mem_free_struct(mem, part);
}

// Fill part with data from str, reducing str size
static void part_fill(Part *part, String *str) {
    u32 len = str->len;
    u32 size_left = array_count(part->data) - part->len;
    if (len > size_left) len = size_left;

    std_memcpy(part->data + part->len, str->data, len);
    part->len += len;

    // Reduce string size
    str->data += len;
    str->len -= len;
}

// Split part at index
// Reduces 'left' size to match offset
// moves data to a new allocated 'right' part
// Returns 'right' part
static Part *part_split(Memory *mem, Part *left, u32 offset) {
    u32 len_left  = offset;
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
    if(left->len + right->len > sizeof(left->data)) return false;
    std_memcpy(left->data + left->len, right->data, right->len);
    left->len += right->len;
    right->len = 0;
    return true;
}

static void part_delete(Part *part, u32 start, u32 count) {
    u32 end = start + count;
    std_memmove(part->data + start, part->data + end, part->len - end);
    part->len -= count;
}

static String part_to_str(Part *part) {
    return (String){part->len, part->data};
}

typedef struct Text Text;
struct Text {
    Memory *mem;
    Part *part;
};

static Text *text_new(Memory *mem) {
    Text *text = mem_struct(mem, Text);
    text->mem = mem;
    return text;
}

static void text_insert(Text *text, u32 position, String str) {
    // Ensure there is at least one part
    if(!text->part) text->part = part_new(text->mem);

    // Find part that contains our position
    // position is in range [0, left->len]
    Part *left = text->part;
    while(position > left->len) {
        position -= left->len;
        left = left->next;
    }

    // If we are in the middle of a part
    // split the part into two
    //             v
    // left = | A B C D | -> ..
    // 
    // left = | A B . . | -> | C D . . | -> ..
    if(position < left->len) {
        Part *right = part_split(text->mem, left, position);
        right->next = left->next;
        left->next = right;
    }

    assert0(position == left->len);

    // Fill left part as much as possible
    part_fill(left, &str);

    // Continue adding more parts until entire string is written
    while (str.len > 0) {
        // Append part
        Part *right = part_new(text->mem);
        right->next = left->next;
        left->next = right;

        // Switch to the new part
        left = right;

        // Fill the part
        part_fill(left, &str);
    }

    // Join previously split right part at the end (if possible)
    Part *right = left->next;
    if (part_join(left, right)) {
        left->next = right->next;
        part_free(text->mem, right);
    }
}


static void text_delete(Text *text, u32 start, u32 count) {
    Part *part = text->part;
    if(!part) return;

    // Find part
    while(start > part->len) {
        part = part->next;
        start -= part->len;
    }

    for(;;) {
        u32 part_count = count;
        u32 max_count = part->len - start;
        if (part_count > max_count) part_count = max_count;
        part_delete(part, start, count);
        // ...
    }
}

// Append entire string
static void text_insert(Text *text, u32 position, String str) {
    part_fill(text->last, &str);
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
