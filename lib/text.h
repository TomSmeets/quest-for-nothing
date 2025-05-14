#pragma once
#include "fmt.h"
#include "mem.h"
#include "str_mem.h"
#include "os_alloc.h"
#include "types.h"
#include "part.h"

// A "Rope" is just a linked list of Strings
// NOTE: this is not (yet) implemented as a rope datastructure
// TODO: implmeent skiplist, this seems like a good structure for this
//
// Always N/2 - N data items
// insert c -> add if fits, other wise split, then add
// 

typedef struct Text Text;
struct Text {
    Memory *mem;

    // Absoulte byte position
    u32 pos;
    u32 line;
    u32 col;

    // Byte position in part
    u32 part_index;
    Part *part;
};

static Text *text_new(Memory *mem) {
    Text *text = mem_struct(mem, Text);
    text->mem = mem;
    text->part = part_new(mem);
    return text;
}


static void text_seek(Text *text, u32 pos) {
    text->pos -= text->part_index;
    text->part_index = 0;
    while (pos <= text->pos && text->pos != 0) {
        text->part = text->part->prev;
        text->pos -= text->part->len;
    }
    while (pos > text->pos + text->part->len) {
        text->part = text->part->next;
        text->pos += text->part->len;
    }
    text->part_index = pos - text->pos;
    text->pos = pos;
}

// Split part at current position
static void _text_split(Text *text) {
    Part *left = text->part;
    Part *right = part_split(text->mem, text->part, text->part_index);
    Part *other = left->next;

    if (other) {
        right->next = other;
        other->prev = right;
    }

    left->next = right;
    right->prev = left;

    assert0(text->part->len == text->part_index);
}

static void _text_fill(Text *text, String *str) {
    String added = part_fill(text->part, *str);
    *str = str_drop_start(*str, added.len);
    text->part_index += added.len;
    text->pos += added.len;
}

static void _text_new_part(Text *text) {
    assert0(text->part_index == text->part->len);

    Part *part = part_new(text->mem);
    Part *left = text->part;
    Part *right = left->next;

    if(right) {
        part->next = right;
        right->prev = part;
    }

    left->next = part;
    part->prev = left;

    text->part = right;
    text->part_index = 0;
}

static void text_insert(Text *text, String str) {
    _text_split(text);
    _text_fill(text, &str);

    while(str.len > 0) {
        _text_new_part(text);
        _text_fill(text, &str);
    }
}

static void text_append(Text *text, String str) {
    if(str.len == 0) return;

    if(!text->part) text->part = part_new(text->mem);

    Part *left = text->part;
    while(left->next) left = left->next;

    // Fill first part
    part_fill(left, &str);

    // Continue adding more parts until entire string is written
    while (str.len > 0) {
        // Append part
        Part *right = part_new(text->mem);
        left->next = right;

        // Switch to the new part
        left = right;

        // Fill the part
        part_fill(left, &str);
    }
}

static void text_insert(Text *text, u32 position, String str) {
    if(str.len == 0) return;

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
        u32 max_count  = part->len - start;
        if (part_count > max_count) part_count = max_count;
        part_delete(part, start, part_count);

        start = 0;
        count -= part_count;
        part = part->next;
        if(count == 0) break;
    }
}


static Text *text_from(Memory *mem, String str) {
    Text *text = text_new(mem);
    text_append(text, str);
    return text;
}

static bool text_eq(Text *text, String str) {
    // For every piece check that the string starts with it
    // then remove that piece from the string until no piecers are left
    for (Part *part = text->part; part;part =part->next) {
        if (!str_drop_start_matching(&str, part_to_str(part))) return false;
    }

    // String was longer than the text
    if(str.len > 0) return false;
    return true;
}

static void text_debug(Text *text, Fmt *fmt) {
    fmt_s(fmt, "Text:\n");
    u32 index = 0;
    for (Part *part = text->part; part; part = part->next) {
        fmt_su(fmt, "=== Part Index ", index++, " ===\n");
        fmt_buf(fmt, (u8 *)part->data, part->len);
        fmt_s(fmt, "\n");
    }
}

// TODO: Text
static void test_text(void) {
    Memory *mem = mem_new();


    Text *text = text_from(mem, S0);
    assert0(text->mem == mem);
    assert0(text->part == 0);

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
        text_insert(text, cursor++, S("0"));
        text_insert(text, cursor++, S("1"));
        text_insert(text, cursor++, S("2"));
        text_insert(text, cursor++, S("3"));
        cursor += len;
    }
    cursor = 21;
    for(u32 i = 0; i < 26 * 2; ++i) {
        text_delete(text, cursor, 4);
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
