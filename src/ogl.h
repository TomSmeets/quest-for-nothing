// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// ogl.h - OpenGL 3.3 helper methods
#pragma once
#include "asset.h"
#include "fmt.h"
#include "image.h"
#include "mat.h"
#include "ogl_api.h"
#include "std.h"
#include "texture_packer.h"
#include "vec.h"

#define OGL_TEXTURE_WIDTH 2048

typedef struct {
    f32 x[3];
    f32 y[3];
    f32 z[3];
    f32 w[3];
    f32 uv_pos[2];
    f32 uv_size[2];
} OGL_Quad;

static_assert(sizeof(OGL_Quad) == 64);

typedef struct {
    OGL_Api api;

    // Vertex Array Object, Stores bound vertex and index buffers
    GLuint vao;

    GLuint vertex_buffer;
    GLuint instance_buffer;

    // Shader Program
    GLuint shader;
    GLint uniform_proj;

    // Texture
    GLuint texture;
    Packer *pack;

    m4s projection_mtx;

    u32 quad_count;
    OGL_Quad quad_list[4096];
} OGL;

// Prevent spam
static u32 ogl_prev_message_index;
static const char *ogl_prev_message[4];

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

static OGL *ogl_load(Memory *mem, void *load(const char *)) {
    OGL *gl = mem_struct(mem, OGL);
    OGL_Api *api = &gl->api;

    // Load OpenGL function pointers
    ogl_api_load(api, load);

    // Debug Output
    api->glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    api->glDebugMessageCallbackARB(ogl_debug_callback, 0);

    // Create VAO
    api->glGenVertexArrays(1, &gl->vao);
    api->glBindVertexArray(gl->vao);

    api->glGenBuffers(1, &gl->vertex_buffer);
    api->glGenBuffers(1, &gl->instance_buffer);

    // Compile Shader
    gl->shader = ogl_program_compile_and_link(api, (char *)FILE_SHADER_VERT, (char *)FILE_SHADER_FRAG);
    gl->uniform_proj = api->glGetUniformLocation(gl->shader, "proj");
    api->glUseProgram(gl->shader);

    // Setup Vertex Buffer
    v2 verts[] = {
        // Top Left
        {0, 1},
        {1, 1},
        {0, 0},
        // Bottom Right
        {1, 0},
        {0, 0},
        {1, 1},
    };

    api->glBindBuffer(GL_ARRAY_BUFFER, gl->vertex_buffer);
    api->glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STREAM_DRAW);

    api->glEnableVertexAttribArray(0);
    api->glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(v2), (void *)0);

    // Setup Instances
    api->glBindBuffer(GL_ARRAY_BUFFER, gl->instance_buffer);

    OGL_Quad *q0 = 0;
    for (u32 i = 1; i <= 6; ++i) {
        api->glEnableVertexAttribArray(i);
        api->glVertexAttribDivisor(i, 1);
    }

    api->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(OGL_Quad), (void *)&q0->x[0]);
    api->glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(OGL_Quad), (void *)&q0->y[0]);
    api->glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(OGL_Quad), (void *)&q0->z[0]);
    api->glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(OGL_Quad), (void *)&q0->w[0]);
    api->glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, sizeof(OGL_Quad), (void *)&q0->uv_pos[0]);
    api->glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, sizeof(OGL_Quad), (void *)&q0->uv_size[0]);

    // Texture atlas
    api->glActiveTexture(GL_TEXTURE0);
    api->glGenTextures(1, &gl->texture);
    api->glBindTexture(GL_TEXTURE_2D, gl->texture);

    api->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    api->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    api->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    api->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // These parameters have to be set for the texture.
    // Otherwise we won't see the textures.
    // NOTE: REQUIRED, https://www.khronos.org/opengl/wiki/Common_Mistakes#Creating_a_complete_texture/
    api->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    api->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    // NOTE: Linear color space
    api->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, OGL_TEXTURE_WIDTH, OGL_TEXTURE_WIDTH, 0, GL_RGBA, GL_FLOAT, 0);
    gl->pack = packer_new(OGL_TEXTURE_WIDTH);

    // Set OpenGL Settings
    api->glEnable(GL_FRAMEBUFFER_SRGB);
    api->glEnable(GL_DEPTH_TEST);
    api->glEnable(GL_MULTISAMPLE);

    if (1) {
        api->glEnable(GL_CULL_FACE);
        api->glCullFace(GL_BACK);
    } else {
        api->glDisable(GL_CULL_FACE);
    }

    if (0) {
        api->glEnable(GL_BLEND);
        api->glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    } else {
        api->glDisable(GL_BLEND);
    }

    api->glCullFace(GL_BACK);
    api->glClearColor(0.02f, 0.02f, 0.02f, 1);
    return gl;
}

static void ogl_begin(OGL *gl, m4s *mtx, v2i viewport_size) {
    gl->quad_count = 0;
    gl->api.glViewport(0, 0, viewport_size.x, viewport_size.y);
    gl->api.glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    gl->projection_mtx = *mtx;
}

static void ogl_draw(OGL *gl) {
    OGL_Api *api = &gl->api;

    // Setup Instances
    api->glBindBuffer(GL_ARRAY_BUFFER, gl->instance_buffer);
    api->glBufferData(GL_ARRAY_BUFFER, sizeof(OGL_Quad) * gl->quad_count, gl->quad_list, GL_STREAM_DRAW);

    // Screen -> Clip
    api->glUniformMatrix4fv(gl->uniform_proj, 1, false, (GLfloat *)&gl->projection_mtx);
    api->glDrawArraysInstanced(GL_TRIANGLES, 0, 6, gl->quad_count);

    gl->quad_count = 0;
}

static void ogl_quad(OGL *gl, m4s *mtx, Image *img) {
    if (gl->quad_count >= array_count(gl->quad_list)) {
        ogl_draw(gl);
        return ogl_quad(gl, mtx, img);
    }

    Packer_Area *area = packer_get_cache(gl->pack, img);

    if (!area) {
        area = packer_get_new(gl->pack, img);

        if (!area) {
            // Draw what is left
            ogl_draw(gl);

            // Recreate texture atlas
            packer_free(gl->pack);
            gl->pack = packer_new(OGL_TEXTURE_WIDTH);

            // Continue
            return ogl_quad(gl, mtx, img);
        };

        gl->api.glTexSubImage2D(GL_TEXTURE_2D, 0, area->pos.x, area->pos.y, img->size.x, img->size.y, GL_RGBA, GL_FLOAT, img->pixels);
    }

    gl->quad_list[gl->quad_count++] = (OGL_Quad){
        .x = {mtx->x.x, mtx->x.y, mtx->x.z},
        .y = {mtx->y.x, mtx->y.y, mtx->y.z},
        .z = {mtx->z.x, mtx->z.y, mtx->z.z},
        .w = {mtx->w.x, mtx->w.y, mtx->w.z},
#if 1
        .uv_pos = {(f32)area->pos.x / OGL_TEXTURE_WIDTH, (f32)area->pos.y / OGL_TEXTURE_WIDTH},
        .uv_size = {(f32)img->size.x / OGL_TEXTURE_WIDTH, (f32)img->size.y / OGL_TEXTURE_WIDTH},
#else
        .uv_size = {1, 1},
#endif
    };
}

static void ogl_sprite(OGL *gl, v3 player, v3 pos, Image *img) {
    v3 dir = pos - player;
    dir.y = 0;

    v3 z = v3_normalize(dir);
    v3 x = {z.z, 0, -z.x};
    v3 y = {0, 1, 0};

    v2 size = v2u_to_v2(img->size) / 32.0;

    m4s mtx = m4s_id();
    mtx.x.xyz = x * size.x;
    mtx.y.xyz = y * size.y;
    mtx.z.xyz = z;
    mtx.w.xyz = pos + (v3){0, size.y * .5f, 0};
    ogl_quad(gl, &mtx, img);
}
