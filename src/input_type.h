// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// input_type.h - User input types
#pragma once
#include "types.h"

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

    bool quit;
} Input;
