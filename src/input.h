// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// input.h - User input types
#pragma once
#include "types.h"
#include "input_type.h"

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

static bool key_down(Input *input, Key key) {
    assert(key < KEY_COUNT, "Invalid key");
    return input->key_state[key];
}

static bool key_click(Input *input, Key key) {
    assert(key < KEY_COUNT, "Invalid key");
    for (u32 i = 0; i < input->key_event_count; ++i) {
        if (input->key_event[i] == key) {
            return true;
        }
    }
    return false;
}

static char key_to_char(Key key) {
    key = key & ~KEY_UP_MASK;
    if (key >= KEY_0 && key <= KEY_9) return key - KEY_0 + '0';
    if (key >= KEY_A && key <= KEY_Z) return key - KEY_A + 'a';
    if (key == ' ') return ' ';
    return '?';
}
