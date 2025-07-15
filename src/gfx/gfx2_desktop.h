#pragma once
#include "gfx/gfx2.h"
#include "gfx/input.h"
#include "gfx/ogl.h"
#include "gfx/ogl_api.h"
#include "gfx/sdl3_api.h"
#include "lib/fmt.h"
#include "qfn/texture_packer.h"

static unsigned char ASSET_SHADER_VERT[] = {
#embed "qfn/gl_shader.vert"
    , 0
};

static unsigned char ASSET_SHADER_FRAG[] = {
#embed "qfn/gl_shader.frag"
    , 0
};

typedef struct {
    f32 x[3];
    f32 y[3];
    f32 z[3];
    f32 w[3];
    f32 uv_pos[2];
    f32 uv_size[2];
} Gfx_Quad;

#define GFX_ATLAS_SIZE 4096

typedef struct Gfx_Pass Gfx_Pass;
struct Gfx_Pass {
    Image *img;
    m4 mtx;
    Gfx_Pass *next;
};

typedef struct {
    Memory *tmp;
    Gfx_Pass *pass_ui;
    Gfx_Pass *pass_3d;
    Packer *pack;
} Gfx_Helper;

static void gfx_help_begin(Gfx_Helper *help, Memory *tmp) {
    help->tmp = tmp;
    help->pass_3d = 0;
    help->pass_ui = 0;
}

static Gfx_Quad gfx_quad_from_mtx(m4 mtx, v2u pos, v2u size) {
    return (Gfx_Quad){
        .x = {mtx.x.x, mtx.x.y, mtx.x.z},
        .y = {mtx.y.x, mtx.y.y, mtx.y.z},
        .z = {mtx.z.x, mtx.z.y, mtx.z.z},
        .w = {mtx.w.x, mtx.w.y, mtx.w.z},
        .uv_pos = {(f32)pos.x / GFX_ATLAS_SIZE, (f32)pos.y / GFX_ATLAS_SIZE},
        .uv_size = {(f32)size.x / GFX_ATLAS_SIZE, (f32)size.y / GFX_ATLAS_SIZE},
    };
}

typedef struct {
    bool need_upload;
    v2u upload_size;
    v2u upload_pos;
    v4 *upload_pixels;
    Gfx_Quad quad;
} Gfx_Help_Fill_Result;

static bool gfx_help_fill(Gfx_Helper *help, Gfx_Help_Fill_Result *result, m4 mtx, Image *img) {
    if (!help->pack) {
        help->pack = packer_new(GFX_ATLAS_SIZE);
    }

    // Check cache
    Packer_Area *area = packer_get_cache(help->pack, img);
    result->need_upload = false;
    if (!area) {
        area = packer_get_new(help->pack, img);
        if (!area) {
            packer_free(help->pack);
            help->pack = 0;
            return false;
        }
        result->need_upload = true;
        result->upload_pos = area->pos;
        result->upload_size = img->size;
        result->upload_pixels = img->pixels;
    }
    result->quad = gfx_quad_from_mtx(mtx, area->pos, img->size);
    return true;
}

static void gfx_help_push(Gfx_Helper *help, bool depth, m4 mtx, Image *img) {
    Gfx_Pass *pass = mem_struct(help->tmp, Gfx_Pass);
    pass->mtx = mtx;
    pass->img = img;
    if (depth) {
        pass->next = help->pass_3d;
        help->pass_3d = pass;
    } else {
        pass->next = help->pass_ui;
        help->pass_ui = pass;
    }
}

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
    Gfx_Helper help;

    Memory *tmp;
    Packer *pack;
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
    gl->glClearColor(0.02f, 0.02f, 0.02f, 1);
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

    gfx_help_begin(&gfx->help, gfx->tmp);
    return input;
}

// Draw image during render
static void gfx_draw(Gfx *gfx, bool depth, m4 mtx, Image *img) {
    gfx_help_push(&gfx->help, depth, mtx, img);
}

static void gfx_draw_pass(Gfx *gfx, Gfx_Pass *pass) {
    OGL_Api *gl = &gfx->gl;
    while (pass) {
        Gfx_Quad quad_list[1024];
        u32 quad_count = 0;
        while (pass) {
            // Out of quads -> Finish pass
            if (quad_count == array_count(quad_list)) break;

            Gfx_Help_Fill_Result result;
            bool ok = gfx_help_fill(&gfx->help, &result, pass->mtx, pass->img);
            if (!ok) break;
            if (result.need_upload) {
                gfx->gl.glTexSubImage2D(
                    GL_TEXTURE_2D, 0, result.upload_pos.x, result.upload_pos.y, result.upload_size.x, result.upload_size.y, GL_RGBA, GL_FLOAT,
                    result.upload_pixels
                );
            }
            quad_list[quad_count++] = result.quad;
            pass = pass->next;
        }
        fmt_su(G->fmt, "count: ", quad_count, "\n");
        if (quad_count > 0) {
            gl->glBufferData(GL_ARRAY_BUFFER, sizeof(Gfx_Quad) * quad_count, quad_list, GL_STREAM_DRAW);
            gl->glDrawArraysInstanced(GL_TRIANGLES, 0, 6, quad_count);
        }
    }
}

static void gfx_end(Gfx *gfx, m4 camera) {
    OGL_Api *gl = &gfx->gl;
    Sdl_Api *sdl = &gfx->sdl;

    v2 aspect = ogl_aspect(gfx->input.window_size);
    m4 view = m4_invert_tr(camera);
    m44 projection = m4_perspective_to_clip(view, 70, aspect.x, aspect.y, 0.1, 15.0);

    // Graphics
    gl->glViewport(0, 0, gfx->input.window_size.x, gfx->input.window_size.y);
    gl->glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    gl->glBindBuffer(GL_ARRAY_BUFFER, gfx->instance_buffer);
    gl->glUniformMatrix4fv(gfx->uniform_proj, 1, false, (GLfloat *)&projection);
    gl->glEnable(GL_DEPTH_TEST);
    gl->glDisable(GL_BLEND);
    gfx_draw_pass(gfx, gfx->help.pass_3d);

    gl->glDisable(GL_DEPTH_TEST);
    gl->glEnable(GL_BLEND);
    gl->glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    gfx_draw_pass(gfx, gfx->help.pass_ui);

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
