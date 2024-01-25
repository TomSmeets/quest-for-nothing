// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// input.h - Defines datatypes for storing per frame user input
#pragma once
#include "inc.h"
#include "math.h"
#include "vec.h"
#include "os.h"

struct input_key {
    u8 down;
    u8 click;
};

enum input_key_code {
    KEY_NONE,

    KEY_0,
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,

    KEY_A,
    KEY_B,
    KEY_C,
    KEY_D,
    KEY_E,
    KEY_F,
    KEY_G,
    KEY_H,
    KEY_I,
    KEY_J,
    KEY_K,
    KEY_L,
    KEY_M,
    KEY_N,
    KEY_O,
    KEY_P,
    KEY_Q,
    KEY_R,
    KEY_S,
    KEY_T,
    KEY_U,
    KEY_V,
    KEY_W,
    KEY_X,
    KEY_Y,
    KEY_Z,

    KEY_MINUS,
    KEY_EQUALS,
    KEY_SPACE,
    KEY_SEMICOLON,
    KEY_SHIFT,

    KEY_MOUSE_LEFT,
    KEY_MOUSE_MIDDLE,
    KEY_MOUSE_RIGHT,

    PAD_A,
    PAD_B,
    PAD_X,
    PAD_Y,

    PAD_BUMPER_LEFT,
    PAD_BUMPER_RIGHT,

    PAD_TRIGGER_LEFT,
    PAD_TRIGGER_RIGHT,

    PAD_UP,
    PAD_DOWN,
    PAD_LEFT,
    PAD_RIGHT,

    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,

    KEY_COUNT
};
typedef enum input_key_code input_key_code;

static_assert(KEY_COUNT <= 256);

// There are multiple 'Controllers', they always belong together
// kbd + mouse
// game pad
// wii remote + nunchuck
struct Input {
    v2 window_size;

    v2 mouse_pos;
    v2 mouse_rel;

    bool quit;
    input_key key[KEY_COUNT];
};
static input_key *input_key_get(Input *in, input_key_code code) {
    if (!code) return 0;
    assert(code < KEY_COUNT);
    return &in->key[code];
}

static void input_emit(Input *in, input_key_code code, bool is_down) {
    input_key *key = input_key_get(in, code);
    if (!key) return;
    key->down  = is_down;
    if(is_down) key->click = 1;
}

static bool input_is_down(Input *in, input_key_code code) {
    input_key *key = input_key_get(in, code);
    return key && key->down;
}

static bool input_is_click(Input *in, input_key_code code) {
    input_key *key = input_key_get(in, code);
    return key && key->click;
}

static void input_update(Input *in) {
    in->mouse_rel = 0;

    for (u32 i = 0; i < array_count(in->key); ++i) {
        input_key *key = in->key + i;
        key->click = 0;
    }
}

static input_key_code input_key_from_char(u32 c) {
    if (c >= 'a' && c <= 'z')
        return KEY_A + (c - 'a');
    if (c >= 'A' && c <= 'Z')
        return KEY_A + (c - 'A');
    if (c >= '0' && c <= '9')
        return KEY_0 + (c - '0');
    if (c == '-')
        return KEY_MINUS;
    if (c == '=')
        return KEY_EQUALS;
    if (c == ' ')
        return KEY_SPACE;
    if (c == ';')
        return KEY_SEMICOLON;
    return KEY_NONE;
}
