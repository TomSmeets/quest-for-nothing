// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// sdl.h - SDL2 wrapper for a single window with both audio and OpenGL
#pragma once
#include "fmt.h"
#include "input.h"
#include "math.h"
#include "mem.h"
#include "os.h"
#include "sdl_api.h"
#include "vec.h"

#define AUDIO_SAMPLE_RATE 48000

// Output audio samples
static void os_gfx_audio_callback(u32 count, v2 *output);

typedef struct {
    Sdl_Api api;
    SDL_Window *win;
    SDL_GLContext *ctx;
    Input input;
    void (*audio_callback)(u32 count, v2 *output);
} Sdl;

static void sdl_audio_callback_wrapper(void *user, u8 *data, i32 size) {
    Sdl *sdl = user;
    sdl->audio_callback((u32)size / sizeof(v2), (v2 *)data);
}

static Sdl *sdl_load(Memory *mem, File *handle, char *title) {
    Sdl *sdl = mem_struct(mem, Sdl);

    // Load sdl function pointers
    Sdl_Api *api = &sdl->api;
    sdl_api_load(api, handle);

#if OS_IS_LINUX
    // Use wayland if possible
    // See: https://www.phoronix.com/news/SDL2-Reverts-Wayland-Default
    api->SDL_SetHint(SDL_HINT_VIDEODRIVER, "wayland,x11");
#endif

    // Init sdl
    api->SDL_Init(SDL_INIT_EVERYTHING);

    // GL attrs must be set before creating the window
    api->SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    api->SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    api->SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG | SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    api->SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    api->SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    api->SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    // Create window
    sdl->win = api->SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    assert(sdl->win, "Failed to create SDL Window");

    // Load OpenGL context
    sdl->ctx = api->SDL_GL_CreateContext(sdl->win);
    assert(sdl->ctx, "Failed to create OpenGL 3.3 Context");
    // gl_load(&sdl->gl, api->SDL_GL_GetProcAddress);

    // Disable vsync
    assert(api->SDL_GL_SetSwapInterval(0) == 0, "Could not disable VSync");

    int window_size_x = 0, window_size_y = 0;
    api->SDL_GetWindowSize(sdl->win, &window_size_x, &window_size_y);
    sdl->input.window_size.x = window_size_x;
    sdl->input.window_size.y = window_size_y;

    // Load Audio
    SDL_AudioSpec want = {
        .freq = AUDIO_SAMPLE_RATE,
        .format = AUDIO_F32,
        .channels = 2,
        .samples = 1024,
        .userdata = sdl,
        .callback = sdl_audio_callback_wrapper,
    };
    sdl->audio_callback = os_gfx_audio_callback;

    assert(api->SDL_OpenAudio(&want, 0) == 0, "Failed to load SDL2 audio");

    // Start Audio
    api->SDL_PauseAudio(0);

    return sdl;
}

static void sdl_quit(Sdl *sdl) {
    Sdl_Api *api = &sdl->api;
    api->SDL_Quit();
}

static Input *sdl_poll(Sdl *sdl) {
    Sdl_Api *api = &sdl->api;

    Input *input = &sdl->input;
    input_reset(input);
    sdl->audio_callback = os_gfx_audio_callback;

    SDL_Event src;
    while (api->SDL_PollEvent(&src)) {
        if (src.type == SDL_QUIT) {
            input->quit = 1;
        }

        if (src.type == SDL_MOUSEMOTION) {
            input->mouse_moved = 1;
            if (input->mouse_is_grabbed) {
                input->mouse_rel.x += src.motion.xrel;
                input->mouse_rel.y -= src.motion.yrel;
            } else {
                input->mouse_pos.x = src.motion.x - input->window_size.x / 2;
                input->mouse_pos.y = input->window_size.y / 2 - src.motion.y;
            }
            // fmt_sffff(
            //     OS_FMT, "Mouse: x=", input->mouse_pos.x, " y=", input->mouse_pos.y, " dx=", input->mouse_rel.x, " dy=", input->mouse_rel.y, "\n"
            // );
        }

        if (src.type == SDL_MOUSEBUTTONDOWN || src.type == SDL_MOUSEBUTTONUP) {
            Key key = KEY_NONE;
            if (src.button.button == SDL_BUTTON_LEFT) key = KEY_MOUSE_LEFT;
            if (src.button.button == SDL_BUTTON_MIDDLE) key = KEY_MOUSE_MIDDLE;
            if (src.button.button == SDL_BUTTON_RIGHT) key = KEY_MOUSE_RIGHT;
            if (src.button.button == SDL_BUTTON_X1) key = KEY_MOUSE_FORWARD;
            if (src.button.button == SDL_BUTTON_X2) key = KEY_MOUSE_BACK;
            input_emit(input, key, src.type == SDL_MOUSEBUTTONDOWN);
        }

        if ((src.type == SDL_KEYDOWN || src.type == SDL_KEYUP) && !src.key.repeat) {
            Key key = KEY_NONE;
            u32 sym = src.key.keysym.sym;
            if (sym >= 'a' && sym <= 'z') key = sym - 'a' + KEY_A;
            if (sym >= '0' && sym <= '9') key = sym - '0' + KEY_0;
            if (sym == ' ') key = KEY_SPACE;
            if (sym == '\e') key = KEY_ESCAPE;
            if (sym == 0x400000e0) key = KEY_CONTROL;
            if (sym == 0x400000e1) key = KEY_SHIFT;
            if (sym == 0x400000e2) key = KEY_ALT;
            if (sym == 0x400000e3) key = KEY_WIN;
            if (sym == 0x400000e4) key = KEY_CONTROL;
            if (sym == 0x400000e5) key = KEY_SHIFT;
            if (sym == 0x400000e6) key = KEY_ALT;
            if (sym == 0x400000e7) key = KEY_WIN;
            if (key == KEY_NONE) fmt_sx(OS_FMT, "SDL_KEY ", sym, "\n");
            input_emit(input, key, src.type == SDL_KEYDOWN);
        }

        if (src.type == SDL_WINDOWEVENT && src.window.event == SDL_WINDOWEVENT_RESIZED) {
            input->window_resized = 1;
            input->window_size.x = src.window.data1;
            input->window_size.y = src.window.data2;
        }

        if (src.type == SDL_WINDOWEVENT && src.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
            input->focus_lost = 1;
        }
    }
    return input;
}

static void sdl_swap_window(Sdl *sdl) {
    Sdl_Api *api = &sdl->api;
    api->SDL_GL_SwapWindow(sdl->win);
}

static void sdl_set_mouse_grab(Sdl *sdl, bool grab) {
    Sdl_Api *api = &sdl->api;
    sdl->input.mouse_is_grabbed = grab;
    api->SDL_SetRelativeMouseMode(grab);
}
