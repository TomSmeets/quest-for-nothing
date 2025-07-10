// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// sdl3.h - SDL3 wrapper for a single window with both audio and OpenGL
#pragma once
// #include <SDL3/SDL.h>
#include "gfx/input.h"
#include "gfx/sdl3_api.h"
#include "lib/fmt.h"
#include "lib/mem.h"

#define AUDIO_SAMPLE_RATE 48000

typedef struct {
    Sdl_Api sdl;
    SDL_Window *window;
    SDL_GLContext gl_context;
    SDL_AudioStream *audio_stream;
    Input input;
    void (*audio_callback)(u32 count, v2 *output);
} Sdl;

static void sdl_audio_callback(u32 count, v2 *output);

static v2 sample_buffer[1024];

static void sdl_audio_callback_wrapper(void *user, SDL_AudioStream *stream, int additional_amount, int total_amount) {
    Sdl *sdl = user;
    u32 total_sample_count = additional_amount / sizeof(v2);
    while (total_sample_count > 0) {
        u32 sample_count = u_min(total_sample_count, array_count(sample_buffer));
        sdl->audio_callback(sample_count, sample_buffer);
        sdl->sdl.SDL_PutAudioStreamData(stream, sample_buffer, sample_count * sizeof(v2));
        total_sample_count -= sample_count;
    }
}

static Sdl *sdl_load(Memory *mem, File *handle, char *title) {
    Sdl *sdl = mem_struct(mem, Sdl);
    sdl_api_load(&sdl->sdl, handle);

#if OS_IS_LINUX
    // Use wayland if possible
    // See: https://www.phoronix.com/news/SDL2-Reverts-Wayland-Default
    sdl->sdl.SDL_SetHint("SDL_VIDEO_DRIVER", "wayland,x11");
#endif

    // Init SDL3 subsystems
    assert0(sdl->sdl.SDL_InitSubSystem(SDL_INIT_EVENTS));
    assert0(sdl->sdl.SDL_InitSubSystem(SDL_INIT_AUDIO));
    assert0(sdl->sdl.SDL_InitSubSystem(SDL_INIT_VIDEO));
    assert0(sdl->sdl.SDL_InitSubSystem(SDL_INIT_GAMEPAD));

    // Configure OpenGL before creating the window
    assert0(sdl->sdl.SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3));
    assert0(sdl->sdl.SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3));
    assert0(sdl->sdl.SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG | SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG));
    assert0(sdl->sdl.SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE));
    assert0(sdl->sdl.SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1));
    assert0(sdl->sdl.SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1));
    assert0(sdl->sdl.SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4));

    // Create window
    SDL_Window *window = sdl->sdl.SDL_CreateWindow(title, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    assert(window, "Failed to create SDL Window");
    sdl->window = window;

    // Load OpenGL context
    SDL_GLContext gl_context = sdl->sdl.SDL_GL_CreateContext(window);
    assert(gl_context, "Failed to create OpenGL 3.3 Context");
    sdl->gl_context = gl_context;

    // Disable VSync
    assert(sdl->sdl.SDL_GL_SetSwapInterval(0), "Failed to disable VSync");

    // Get Initial window size
    int window_size_x = 0, window_size_y = 0;
    assert0(sdl->sdl.SDL_GetWindowSize(window, &window_size_x, &window_size_y));
    sdl->input.window_size.x = window_size_x;
    sdl->input.window_size.y = window_size_y;

    // Load Audio
    const SDL_AudioSpec audio_spec = {
        .format = SDL_AUDIO_F32,
        .channels = 2,
        .freq = AUDIO_SAMPLE_RATE,
    };
    SDL_AudioStream *audio_stream =
        sdl->sdl.SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &audio_spec, sdl_audio_callback_wrapper, sdl);
    assert(audio_stream, "Failed to load Audio");
    sdl->audio_stream = audio_stream;
    sdl->audio_callback = sdl_audio_callback;

    // Start Audio
    SDL_AudioDeviceID audio_device = sdl->sdl.SDL_GetAudioStreamDevice(audio_stream);
    assert0(sdl->sdl.SDL_ResumeAudioDevice(audio_device));
    return sdl;
}

static void sdl_quit(Sdl *sdl) {
    sdl->sdl.SDL_Quit();
}

static u32 sdl_audio_needed(Sdl *sdl) {
    u32 total = AUDIO_SAMPLE_RATE / 30;
    u32 queued = sdl->sdl.SDL_GetAudioStreamQueued(sdl->audio_stream) / sizeof(v2);
    if (queued > total) return 0;
    return total - queued;
}

static void sdl_audio_put(Sdl *sdl, u32 sample_count, v2 *sample_list) {
    assert0(sdl->sdl.SDL_PutAudioStreamData(sdl->audio_stream, sample_list, sample_count * sizeof(v2)));
}

static Input *sdl_poll(Sdl *sdl) {
    // Update audio callback
    if (G->reloaded) sdl->audio_callback = sdl_audio_callback;

    Input *input = &sdl->input;
    input_reset(input);

    SDL_Event event;
    while (sdl->sdl.SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_EVENT_QUIT: {
            input->quit = 1;
        } break;

        case SDL_EVENT_MOUSE_MOTION: {
            input->mouse_moved = 1;
            if (input->mouse_is_grabbed) {
                input->mouse_rel.x += event.motion.xrel;
                input->mouse_rel.y -= event.motion.yrel;
            } else {
                input->mouse_pos.x = event.motion.x - input->window_size.x / 2;
                input->mouse_pos.y = input->window_size.y / 2 - event.motion.y;
            }
        } break;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP: {
            Key key = KEY_NONE;
            if (event.button.button == SDL_BUTTON_LEFT) key = KEY_MOUSE_LEFT;
            if (event.button.button == SDL_BUTTON_MIDDLE) key = KEY_MOUSE_MIDDLE;
            if (event.button.button == SDL_BUTTON_RIGHT) key = KEY_MOUSE_RIGHT;
            if (event.button.button == SDL_BUTTON_X1) key = KEY_MOUSE_FORWARD;
            if (event.button.button == SDL_BUTTON_X2) key = KEY_MOUSE_BACK;
            input_emit(input, key, event.button.down);
        } break;

        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP: {
            if (event.key.repeat) break;

            Key key = KEY_NONE;
            u32 sdlk = event.key.key;
            if (sdlk >= SDLK_A && sdlk <= SDLK_Z) key = sdlk - 'a' + KEY_A;
            if (sdlk >= SDLK_0 && sdlk <= SDLK_9) key = sdlk - '0' + KEY_0;
            if (sdlk == SDLK_SPACE) key = KEY_SPACE;
            if (sdlk == SDLK_ESCAPE) key = KEY_ESCAPE;
            if (sdlk == SDLK_LCTRL || sdlk == SDLK_RCTRL) key = KEY_CONTROL;
            if (sdlk == SDLK_LSHIFT || sdlk == SDLK_RSHIFT) key = KEY_SHIFT;
            if (sdlk == SDLK_LALT || sdlk == SDLK_RALT) key = KEY_ALT;
            if (sdlk == SDLK_LGUI || sdlk == SDLK_RGUI) key = KEY_WIN;
            if (key == KEY_NONE) fmt_sx(G->fmt, "SDLK ", sdlk, "\n");
            input_emit(input, key, event.key.down);
        } break;

        case SDL_EVENT_WINDOW_RESIZED: {
            input->window_resized = 1;
            input->window_size.x = event.window.data1;
            input->window_size.y = event.window.data2;
        } break;

        case SDL_EVENT_WINDOW_FOCUS_LOST: {
            input->focus_lost = 1;
        } break;
        }
    }
    return input;
}

static void sdl_swap_window(Sdl *sdl) {
    sdl->sdl.SDL_GL_SwapWindow(sdl->window);
}

static void sdl_set_mouse_grab(Sdl *sdl, bool grab) {
    sdl->input.mouse_is_grabbed = grab;
    sdl->sdl.SDL_SetWindowRelativeMouseMode(sdl->window, grab);
}

static void sdl_set_fullscreen(Sdl *sdl, bool fullscreen) {
    sdl->input.is_fullscreen = fullscreen;
    sdl->sdl.SDL_SetWindowFullscreen(sdl->window, fullscreen);
}
