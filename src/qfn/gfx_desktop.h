// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// gfx_desktop.h - GFX Platform implementation for Windows and Linux
#pragma once
#include "gfx/ogl.h"
#include "qfn/gfx.h"
#include "qfn/sdl3.h"

static unsigned char ASSET_SHADER_VERT[] = {
#embed "qfn/gl_shader.vert"
    , 0
};

static unsigned char ASSET_SHADER_FRAG[] = {
#embed "qfn/gl_shader.frag"
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

// Initialize Graphics stack
static Gfx_Imp *gfx_imp_init(Memory *mem, char *title) {
    Gfx_Imp *gfx = mem_struct(mem, Gfx_Imp);
    OGL_Api *gl = &gfx->gl;

    // Load SDL2
    File *lib = os_dlopen(OS_IS_LINUX ? S("libSDL3.so") : S("SDL3.dll"));
    gfx->sdl = sdl_load(mem, lib, title);

    // Load OpenGL function pointers
    ogl_api_load(gl, gfx->sdl->sdl.SDL_GL_GetProcAddress);
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

// Start frame
static Input *gfx_imp_begin(Gfx_Imp *gfx) {
    Input *input = sdl_poll(gfx->sdl);
    gfx->gl.glViewport(0, 0, input->window_size.x, input->window_size.y);
    gfx->gl.glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    return input;
}

// Grab mouse
static void gfx_set_grab(Gfx *gfx, bool grab) {
    sdl_set_mouse_grab(gfx->os->sdl, grab);
}

static void gfx_set_fullscreen(Gfx *gfx, bool fullscreen) {
    sdl_set_fullscreen(gfx->os->sdl, fullscreen);
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
