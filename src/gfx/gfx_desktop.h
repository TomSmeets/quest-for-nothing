// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// gfx_deskitop.h - Gfx implentation for Linux and Windows desktop
#pragma once
#include "gfx/gfx.h"
#include "gfx/gfx_help.h"
#include "gfx/input.h"
#include "gfx/ogl.h"
#include "gfx/ogl_api.h"
#include "gfx/sdl3_api.h"
#include "gfx/texture_packer.h"
#include "lib/fmt.h"

static unsigned char ASSET_SHADER_VERT[] = {
#embed "gfx/gl_shader.vert"
    , 0
};

static unsigned char ASSET_SHADER_FRAG[] = {
#embed "gfx/gl_shader.frag"
    , 0
};

struct Gfx {
    Input input;

    // Sdl2
    Sdl_Api sdl;
    SDL_Window *window;
    SDL_GLContext gl_context;
    bool audio_started;
    SDL_AudioStream *audio_stream;
    void (*audio_callback)(u32 count, v2 *output);
    v2 sample_buffer[1024];

    // OpenGL 3.3
    OGL_Api gl;

    // Vertex Array Object, Stores bound vertex and index buffers
    GLuint vao;
    GLuint instance_buffer;

    // Shader Program
    GLuint shader;
    GLint uniform_proj;

    // Texture
    GLuint texture;

    Memory *tmp;
    Packer *pack;
    Gfx_Pass_List pass_3d;
    Gfx_Pass_List pass_ui;
};

static void sdl_audio_callback_wrapper(void *user, SDL_AudioStream *stream, int additional_amount, int total_amount) {
    Gfx *gfx = user;
    u32 total_sample_count = additional_amount / sizeof(v2);
    while (total_sample_count > 0) {
        u32 sample_count = u_min(total_sample_count, array_count(gfx->sample_buffer));
        gfx->audio_callback(sample_count, gfx->sample_buffer);
        gfx->sdl.SDL_PutAudioStreamData(stream, gfx->sample_buffer, sample_count * sizeof(v2));
        total_sample_count -= sample_count;
    }
}

static Gfx *gfx_init(Memory *mem, const char *title) {
    Gfx *gfx = mem_struct(mem, Gfx);

    // SDL3 windowing
    File *lib_sdl = os_dlopen(OS_IS_LINUX ? S("libSDL3.so") : S("SDL3.dll"));
    sdl_api_load(&gfx->sdl, lib_sdl);

#if OS_IS_LINUX
    // Use wayland if possible
    // See: https://www.phoronix.com/news/SDL2-Reverts-Wayland-Default
    gfx->sdl.SDL_SetHint("SDL_VIDEO_DRIVER", "wayland,x11");
#endif

    // Init SDL3 subsystems
    assert0(gfx->sdl.SDL_InitSubSystem(SDL_INIT_EVENTS));
    assert0(gfx->sdl.SDL_InitSubSystem(SDL_INIT_AUDIO));
    assert0(gfx->sdl.SDL_InitSubSystem(SDL_INIT_VIDEO));
    assert0(gfx->sdl.SDL_InitSubSystem(SDL_INIT_GAMEPAD));

    // Configure OpenGL before creating the window
    assert0(gfx->sdl.SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3));
    assert0(gfx->sdl.SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3));
    assert0(gfx->sdl.SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG | SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG));
    assert0(gfx->sdl.SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE));
    assert0(gfx->sdl.SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1));
    assert0(gfx->sdl.SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1));
    assert0(gfx->sdl.SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4));

    // Create window
    SDL_Window *window = gfx->sdl.SDL_CreateWindow(title, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    assert(window, "Failed to create SDL Window");
    gfx->window = window;

    // Load OpenGL context
    SDL_GLContext gl_context = gfx->sdl.SDL_GL_CreateContext(window);
    assert(gl_context, "Failed to create OpenGL 3.3 Context");
    gfx->gl_context = gl_context;

    // Disable VSync
    assert(gfx->sdl.SDL_GL_SetSwapInterval(0), "Failed to disable VSync");

    // Get Initial window size
    int window_size_x = 0, window_size_y = 0;
    assert0(gfx->sdl.SDL_GetWindowSize(window, &window_size_x, &window_size_y));
    gfx->input.window_size.x = window_size_x;
    gfx->input.window_size.y = window_size_y;

    // Load Audio
    const SDL_AudioSpec audio_spec = {
        .format = SDL_AUDIO_F32,
        .channels = 2,
        .freq = GFX_AUDIO_RATE,
    };
    SDL_AudioStream *audio_stream =
        gfx->sdl.SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &audio_spec, sdl_audio_callback_wrapper, gfx);
    assert(audio_stream, "Failed to load Audio");
    gfx->audio_stream = audio_stream;
    gfx->audio_callback = gfx_audio_callback;

    // Load OpenGL function pointers
    OGL_Api *gl = &gfx->gl;
    ogl_api_load(gl, gfx->sdl.SDL_GL_GetProcAddress);
    ogl_enable_debug(gl);

    // Create VAO
    gl->glGenVertexArrays(1, &gfx->vao);
    gl->glBindVertexArray(gfx->vao);
    gl->glGenBuffers(1, &gfx->instance_buffer);

    // Compile Shader
    gfx->shader = ogl_program_compile_and_link(gl, (char *)ASSET_SHADER_VERT, (char *)ASSET_SHADER_FRAG);
    gfx->uniform_proj = gl->glGetUniformLocation(gfx->shader, "proj");
    gl->glUseProgram(gfx->shader);

    // Setup Instances
    gl->glBindBuffer(GL_ARRAY_BUFFER, gfx->instance_buffer);

    Gfx_Quad *q0 = 0;
    for (u32 i = 0; i <= 5; ++i) {
        gl->glEnableVertexAttribArray(i);
        gl->glVertexAttribDivisor(i, 1);
    }
    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(*q0), (void *)&q0->x[0]);
    gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(*q0), (void *)&q0->y[0]);
    gl->glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(*q0), (void *)&q0->z[0]);
    gl->glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(*q0), (void *)&q0->w[0]);
    gl->glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(*q0), (void *)&q0->uv_pos[0]);
    gl->glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, sizeof(*q0), (void *)&q0->uv_size[0]);

    // Texture atlas
    gl->glActiveTexture(GL_TEXTURE0);
    gl->glGenTextures(1, &gfx->texture);
    gl->glBindTexture(GL_TEXTURE_2D, gfx->texture);

    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // These parameters have to be set for the texture.
    // Otherwise we won't see the textures.
    // NOTE: REQUIRED, https://www.khronos.org/opengl/wiki/Common_Mistakes#Creating_a_complete_texture/
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    // NOTE: Linear color space
    gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, GFX_ATLAS_SIZE, GFX_ATLAS_SIZE, 0, GL_RGBA, GL_FLOAT, 0);

    // Set OpenGL Settings
    gl->glEnable(GL_FRAMEBUFFER_SRGB);
    gl->glEnable(GL_MULTISAMPLE);
    gl->glEnable(GL_CULL_FACE);
    gl->glCullFace(GL_BACK);
    gl->glClearColor(0.1f, 0.1f, 0.2f, 1);
    gl->glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    gl->glEnable(GL_SAMPLE_SHADING);
    gl->glMinSampleShading(1);
    return gfx;
}

static Input *gfx_begin(Gfx *gfx) {
    gfx->tmp = mem_new();

    // Update audio callback
    if (G->reloaded || !gfx->audio_callback) gfx->audio_callback = gfx_audio_callback;

    // Start Audio
    if (!gfx->audio_started) {
        gfx->audio_started = 1;
        SDL_AudioDeviceID audio_device = gfx->sdl.SDL_GetAudioStreamDevice(gfx->audio_stream);
        assert0(gfx->sdl.SDL_ResumeAudioDevice(audio_device));
    }

    Input *input = &gfx->input;
    input_reset(input);

    SDL_Event event;
    while (gfx->sdl.SDL_PollEvent(&event)) {
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

    gfx->pass_3d = (Gfx_Pass_List){};
    gfx->pass_ui = (Gfx_Pass_List){};
    return input;
}

// Draw image during render
static void gfx_draw_3d(Gfx *gfx, m4 mtx, Image *img) {
    gfx_pass_push(gfx->tmp, &gfx->pass_3d, mtx, img);
}

static void gfx_draw_ui(Gfx *gfx, m4 mtx, Image *img) {
    gfx_pass_push(gfx->tmp, &gfx->pass_ui, mtx, img);
}

static void gfx_draw_pass(Gfx *gfx, Gfx_Pass_List *pass) {
    OGL_Api *gl = &gfx->gl;
    Gfx_Pass_Compiled result;
    while (gfx_pass_compile(&result, &gfx->pack, pass)) {
        // fmt_su(G->fmt, "Upload = ", result.upload_count, "\n");
        // fmt_su(G->fmt, "Draw   = ", result.quad_count, "\n");
        for (u32 i = 0; i < result.upload_count; ++i) {
            Gfx_Upload *upload = result.upload_list + i;
            u32 x = upload->pos.x;
            u32 y = upload->pos.y;
            u32 w = upload->size.x;
            u32 h = upload->size.y;
            gl->glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, w, h, GL_RGBA, GL_FLOAT, upload->pixels);
        }
        gl->glBufferData(GL_ARRAY_BUFFER, sizeof(Gfx_Quad) * result.quad_count, result.quad_list, GL_STREAM_DRAW);
        gl->glDrawArraysInstanced(GL_TRIANGLES, 0, 6, result.quad_count);
    }
}

static void gfx_end(Gfx *gfx, m4 camera) {
    OGL_Api *gl = &gfx->gl;
    Sdl_Api *sdl = &gfx->sdl;

    v2 aspect = ogl_aspect(gfx->input.window_size);
    m4 view = m4_invert_tr(camera);
    m44 projection = m4_perspective_to_clip(view, 70, aspect.x, aspect.y, 0.1, 15.0);

    // Graphics
    gl->glClearColor(0.2f, 0.3f, 0.4f, 1);
    gl->glViewport(0, 0, gfx->input.window_size.x, gfx->input.window_size.y);
    gl->glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    gl->glBindBuffer(GL_ARRAY_BUFFER, gfx->instance_buffer);
    gl->glUniformMatrix4fv(gfx->uniform_proj, 1, false, (GLfloat *)&projection);
    gl->glEnable(GL_DEPTH_TEST);
    gl->glDisable(GL_BLEND);
    gfx_draw_pass(gfx, &gfx->pass_3d);

    m44 screen = m4_screen_to_clip(m4_id(), gfx->input.window_size);
    gl->glUniformMatrix4fv(gfx->uniform_proj, 1, false, (GLfloat *)&screen);
    gl->glDisable(GL_DEPTH_TEST);
    gl->glEnable(GL_BLEND);
    gl->glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    gfx_draw_pass(gfx, &gfx->pass_ui);

    // Swap
    sdl->SDL_GL_SwapWindow(gfx->window);

    mem_free(gfx->tmp);
    gfx->tmp = 0;
}

// Set mouse grab
static void gfx_set_grab(Gfx *gfx, bool grab) {
    gfx->input.mouse_is_grabbed = grab;
    gfx->sdl.SDL_SetWindowRelativeMouseMode(gfx->window, grab);
}

// Set Window fullscreen
static void gfx_set_fullscreen(Gfx *gfx, bool fullscreen) {
    gfx->input.is_fullscreen = fullscreen;
    gfx->sdl.SDL_SetWindowFullscreen(gfx->window, fullscreen);
}
