// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// input.h - User input types
#pragma once
#include "lib/types.h"
#include "lib/vec.h"

typedef enum {
    KEY_NONE,

    // Keys
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

    // Special Keys
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_SPACE,
    KEY_ESCAPE,

    // Mouse
    KEY_MOUSE_LEFT,
    KEY_MOUSE_MIDDLE,
    KEY_MOUSE_RIGHT,
    KEY_MOUSE_FORWARD,
    KEY_MOUSE_BACK,

    // Mods
    KEY_SHIFT,
    KEY_CONTROL,
    KEY_WIN,
    KEY_ALT,

    KEY_COUNT,

    // Masks
    KEY_UP_MASK = 0x10000000,
} Key;

typedef struct {
    u8 key_state[KEY_COUNT];

    u32 key_event_count;
    Key key_event[32];

    bool mouse_moved;
    v2i mouse_pos;
    v2i mouse_rel;

    bool window_resized;
    v2i window_size;
    bool focus_lost;

    // Events
    bool mouse_is_grabbed;
    bool is_fullscreen;

    bool quit;
} Input;

static void input_reset(Input *input) {
    input->key_event_count = 0;
    input->mouse_moved = 0;
    input->mouse_rel.x = 0;
    input->mouse_rel.y = 0;
    input->window_resized = 0;
    input->quit = 0;
    input->focus_lost = 0;
}

static void input_emit(Input *input, Key key, bool down) {
    if (!key) return;
    bool was_down = input->key_state[key];

    // Skip if it was already down or up
    if (down == was_down) return;

    if (input->key_event_count < array_count(input->key_event)) {
        input->key_event[input->key_event_count++] = down ? key : key | KEY_UP_MASK;
    }

    input->key_state[key] = down;
}

static bool input_down(Input *input, Key key) {
    assert(key < KEY_COUNT, "Invalid key");
    return input->key_state[key];
}

static bool input_click(Input *input, Key key) {
    assert(key < KEY_COUNT, "Invalid key");
    for (u32 i = 0; i < input->key_event_count; ++i) {
        if (input->key_event[i] == key) {
            return true;
        }
    }
    return false;
}

static void input_toggle(Input *input, Key key, bool *toggle) {
    if (input_click(input, key)) {
        *toggle = !*toggle;
    }
}

static void input_cycle(Input *input, Key key, u32 *value, u32 max) {
    if (input_click(input, key)) {
        *value = (*value + 1) % max;
    }
}

static char key_to_char(Key key) {
    key = key & ~KEY_UP_MASK;
    if (key >= KEY_0 && key <= KEY_9) return key - KEY_0 + '0';
    if (key >= KEY_A && key <= KEY_Z) return key - KEY_A + 'a';
    if (key == ' ') return ' ';
    return '?';
}

static Key key_from_char(u32 c) {
    if (c >= 'a' && c <= 'z') return KEY_A + (c - 'a');
    if (c >= 'A' && c <= 'Z') return KEY_A + (c - 'A');
    if (c >= '0' && c <= '9') return KEY_0 + (c - '0');
    if (c == ' ') return KEY_SPACE;
    return KEY_NONE;
}
