// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// gfx_desktop.h - GFX Platform implementation for Windows and Linux
#pragma once
#include "gfx.h"
#include "ogl_api.h"
#include "sdl.h"

static unsigned char ASSET_SHADER_VERT[] = {
#embed "gl_shader.vert"
    , 0
};

static unsigned char ASSET_SHADER_FRAG[] = {
#embed "gl_shader.frag"
    , 0
};

struct Gfx_Imp {
    OGL_Api gl;
    Sdl *sdl;

    // Vertex Array Object, Stores bound vertex and index buffers
    GLuint vao;
    GLuint instance_buffer;

    // Shader Program
    GLuint shader;
    GLint uniform_proj;

    // Texture
    GLuint texture;
};

// Prevent spam
static u32 ogl_prev_message_index;
static const char *ogl_prev_message[4];

static void ogl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *user) {
    // Stop spam
    for (u32 i = 0; i < array_count(ogl_prev_message); ++i) {
        if (message == ogl_prev_message[i]) return;
    }

    char *prefix = "????";
    if (severity == GL_DEBUG_SEVERITY_HIGH) prefix = "HIGH";
    if (severity == GL_DEBUG_SEVERITY_MEDIUM) prefix = "MEDIUM";
    if (severity == GL_DEBUG_SEVERITY_LOW) prefix = "LOW";
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) prefix = "NOTIFICATION";
    fmt_sss(OS_FMT, "[", prefix, "] ", (char *)message, "\n");
    ogl_prev_message[ogl_prev_message_index] = message;
    ogl_prev_message_index++;
    if (ogl_prev_message_index >= 4) ogl_prev_message_index = 0;
}

static GLuint ogl_compile_shader(OGL_Api *gl, GLenum type, char *source) {
    GLuint shader = gl->glCreateShader(type);
    gl->glShaderSource(shader, 1, (const char *const[]){source}, 0);
    gl->glCompileShader(shader);

    i32 success;
    gl->glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char buffer[1024 * 4];
        gl->glGetShaderInfoLog(shader, sizeof(buffer), 0, buffer);
        char *shader_type = 0;
        if (type == GL_VERTEX_SHADER) shader_type = "vertex";
        if (type == GL_FRAGMENT_SHADER) shader_type = "fragment";
        fmt_sss(OS_FMT, "error while compiling the ", shader_type, " shader: ", buffer, "\n");
        return 0;
    }

    return shader;
}

static GLuint ogl_link_program(OGL_Api *gl, GLuint vertex, GLuint fragment) {
    // link shaders
    GLuint program = gl->glCreateProgram();
    gl->glAttachShader(program, vertex);
    gl->glAttachShader(program, fragment);
    gl->glLinkProgram(program);

    // check for linking errors
    i32 success;
    gl->glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char buffer[1024 * 4];
        gl->glGetProgramInfoLog(program, sizeof(buffer), 0, buffer);
        fmt_ss(OS_FMT, "error while linking shader: ", buffer, "\n");
        return 0;
    }

    return program;
}

static GLuint ogl_program_compile_and_link(OGL_Api *gl, char *vert, char *frag) {
    GLuint vert_shader = ogl_compile_shader(gl, GL_VERTEX_SHADER, vert);
    GLuint frag_shader = ogl_compile_shader(gl, GL_FRAGMENT_SHADER, frag);
    if (!vert_shader || !frag_shader) return 0;

    GLuint shader_program = ogl_link_program(gl, vert_shader, frag_shader);
    gl->glDeleteShader(vert_shader);
    gl->glDeleteShader(frag_shader);
    return shader_program;
}

// Initialize Graphics stack
static Gfx_Imp *gfx_imp_init(Memory *mem, char *title) {
    Gfx_Imp *gfx = mem_struct(mem, Gfx_Imp);
    OGL_Api *gl = &gfx->gl;

    // Load SDL2
    File *lib = os_dlopen(OS_IS_LINUX ? "libSDL2.so" : "SDL2.dll");
    gfx->sdl = sdl_load(mem, lib, title);

    // Load OpenGL function pointers
    ogl_api_load(gl, gfx->sdl->api.SDL_GL_GetProcAddress);

    // Debug Output
    gl->glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    gl->glDebugMessageCallbackARB(ogl_debug_callback, 0);

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

    GLint maxSamples;
    gl->glGetIntegerv(GL_MAX_SAMPLES, &maxSamples);
    fmt_si(OS_FMT, "OGL: Maximum supported multisample samples: ", maxSamples, "\n");
    return gfx;
}

// Start frame
static Input *gfx_imp_begin(Gfx_Imp *gfx) {
    Input *input = sdl_poll(gfx->sdl);
    gfx->gl.glViewport(0, 0, input->window_size.x, input->window_size.y);
    gfx->gl.glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    return input;
}

// Grab mouse
static void gfx_imp_set_grab(Gfx_Imp *gfx, bool grab) {
    sdl_set_mouse_grab(gfx->sdl, grab);
}

static void gfx_imp_set_fullscreen(Gfx_Imp *gfx, bool full) {
    gfx->sdl->api.SDL_SetWindowFullscreen(gfx->sdl->win, full ? SDL_WINDOW_FULLSCREEN : 0);
    gfx->sdl->input.is_fullscreen = full;
}

// Write to texture atlas
static void gfx_imp_texture(Gfx_Imp *gfx, v2u pos, Image *img) {
    gfx->gl.glTexSubImage2D(GL_TEXTURE_2D, 0, pos.x, pos.y, img->size.x, img->size.y, GL_RGBA, GL_FLOAT, img->pixels);
}

// Perform a draw call
static void gfx_imp_draw(Gfx_Imp *gfx, m44 projection, bool depth, u32 quad_count, Gfx_Quad *quad_list) {
    OGL_Api *api = &gfx->gl;

    if (depth) {
        api->glEnable(GL_DEPTH_TEST);
        api->glDisable(GL_BLEND);
    } else {
        api->glDisable(GL_DEPTH_TEST);
        api->glEnable(GL_BLEND);
        api->glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    }

    api->glBindBuffer(GL_ARRAY_BUFFER, gfx->instance_buffer);
    api->glBufferData(GL_ARRAY_BUFFER, sizeof(quad_list[0]) * quad_count, quad_list, GL_STREAM_DRAW);
    api->glUniformMatrix4fv(gfx->uniform_proj, 1, false, (GLfloat *)&projection);
    api->glDrawArraysInstanced(GL_TRIANGLES, 0, 6, quad_count);
}

// Finish frame
static void gfx_imp_end(Gfx_Imp *gfx) {
    sdl_swap_window(gfx->sdl);
}

static void sdl_audio_callback(u32 count, v2 *output) {
    gfx_audio_callback(count, output);
}
