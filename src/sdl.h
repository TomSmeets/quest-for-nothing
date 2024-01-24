// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// sdl.h - SDL2 wrapper for a single window with both audio and opengl
#pragma once

#include "fmt.h"
#include "gl.h"
#include "inc.h"
#include "input.h"
#include "math.h"
#include "mem.h"
#include "sdl_api.h"

#include <stdlib.h>

typedef void SDL_Audio_Callback(f32 dt, u32 count, v2 *output);

struct Sdl {
    sdl_api api;
    SDL_Window *win;

    // video
    SDL_GLContext *ctx;
    gl_api *gl;

    // audio
    f32 audio_dt;
    SDL_Audio_Callback *audio_callback;

    // input
    v2 mouse_pos;
    v2 mouse_rel;

    Input input;
};

// TODO: locking?? -> just expose sdl_lock and sdl_unlock audio
static void sdl_call_audio_callback(void *user, u8 *data, i32 len) {
    Sdl *sdl = user;
    f32 dt = sdl->audio_dt;
    u32 output_count = len / sizeof(v2);
    v2 *output = (v2 *)data;

    if (sdl->audio_callback) {
        sdl->audio_callback(dt, output_count, output);
        for (u32 i = 0; i < output_count; ++i) {
            output[i] *= 0.2;
            output[i].x = f_clamp(output[i].x, -1, 1);
            output[i].y = f_clamp(output[i].y, -1, 1);
        }
    } else {
        std_memzero(data, len);
    }
}

// Create a new sdl instance
static Sdl *sdl_new(mem *m, const char *title) {
    Sdl *sdl = mem_struct(m, Sdl);
    sdl_api *api = &sdl->api;
    sdl_api_load(api);

    // Use wayland if possible
    // See: https://www.phoronix.com/news/SDL2-Reverts-Wayland-Default
    api->SDL_SetHint(SDL_HINT_VIDEODRIVER, "wayland,x11");

    // init sdl
    api->SDL_Init(SDL_INIT_EVERYTHING);

    // gl attrs HAVE to be set before creating the window
    api->SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    api->SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    api->SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG | SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    api->SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    api->SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    api->SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    // init window
    sdl->win = api->SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        800, 600,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );
    assert(sdl->win);

    // Load opengl context
    sdl->ctx = api->SDL_GL_CreateContext(sdl->win);
    assert(sdl->ctx);

    // disable vsync
    assert(api->SDL_GL_SetSwapInterval(0) == 0);

    // load opengl ptrs
    sdl->gl = gl_load(m, api->SDL_GL_GetProcAddress);
    assert(sdl->gl);

    // load Audio
    u32 sample_rate = 48000;
    u32 audio_buffer_size = sample_rate / 200;
    SDL_AudioSpec want = {
        .freq     = sample_rate,
        .format   = AUDIO_F32,
        .channels = 2,
        .samples  = audio_buffer_size,
        .userdata = sdl,
        .callback = sdl_call_audio_callback,
    };
    sdl->audio_dt = 1.0f / sample_rate;
    assert(api->SDL_OpenAudio(&want, 0) == 0);
    api->SDL_PauseAudio(0);
    return sdl;
}

static void sdl_quit(Sdl *sdl) {
    sdl_api *api = &sdl->api;
    api->SDL_Quit();
}

static input_key_code sdl_translate_key(SDL_Keycode code) {
    if (code == 0x400000e1) return KEY_SHIFT;
    return input_key_from_char(code);
}

static void sdl_begin(Sdl *sdl) {
    sdl_api *api = &sdl->api;

    input_update(&sdl->input);

    // -- input --
    SDL_Event event;
    while (api->SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            sdl->input.quit = 1;
        }

        if (event.type == SDL_MOUSEMOTION) {
            sdl->input.mouse_pos.x = event.motion.x;
            sdl->input.mouse_pos.y = event.motion.y;
            sdl->input.mouse_rel.x += event.motion.xrel;
            sdl->input.mouse_rel.y += event.motion.yrel;
        }

        if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP) {
            u8 state = event.button.state;
            switch (event.button.button) {
            case SDL_BUTTON_LEFT:   input_emit(&sdl->input, KEY_MOUSE_LEFT, state); break;
            case SDL_BUTTON_MIDDLE: input_emit(&sdl->input, KEY_MOUSE_MIDDLE, state); break;
            case SDL_BUTTON_RIGHT:  input_emit(&sdl->input, KEY_MOUSE_RIGHT, state); break;
            }
        }

        if ((event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) && !event.key.repeat) {
            u32 sym = event.key.keysym.sym;
            input_key_code key = sdl_translate_key(event.key.keysym.sym);
            input_emit(&sdl->input, key, event.key.state);
        }
    }

    i32 x = 0, y = 0;
    api->SDL_GetWindowSize(sdl->win, &x, &y);
    sdl->input.window_size.x = x;
    sdl->input.window_size.y = y;
}

static void sdl_end(Sdl *sdl) {
    sdl_api *api = &sdl->api;
    api->SDL_GL_SwapWindow(sdl->win);
}
