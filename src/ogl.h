// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// gl.h - OpenGL 3.3 helper methods
#pragma once
#include "asset.h"
#include "fmt.h"
#include "ogl_api.h"
#include "image.h"
#include "mat.h"
#include "std.h"
#include "vec.h"

#define OGL_TEXTURE_WIDTH 2048
#define OGL_TILE_WIDTH 32

#define OGL_TEXTURE_WIDTH_IN_TILES (OGL_TEXTURE_WIDTH / OGL_TILE_WIDTH)
#define OGL_TEXTURE_SIZE_IN_TILES (OGL_TEXTURE_WIDTH_IN_TILES*OGL_TEXTURE_WIDTH_IN_TILES)

typedef struct {
    f32 pos[3];
    u8 texture[3];
} OGL_Quad;

static_assert(sizeof(OGL_Quad) == 16);

typedef struct {
    OGL_Api api;

    // Vertex Array Object, Stores bound vertex and index buffers
    GLuint vao;

    GLuint vertex_buffer;
    GLuint instance_buffer;

    // Shader Program
    GLuint shader;
    GLint uniform_proj;
    GLint uniform_camera_pos;

    // Texture
    GLuint texture;

    // Some single texture limits:
    //   1024 x   1024   / (32 x 32)  =  32 x  32   =   1024
    //   2048 x   2048   / (32 x 32)  =  64 x  64   =   4096
    // 262144 x 262144   / (32 x 32)  = 512 x 512   = 262144
    //
    // Multiple textures are ofcourse also always possible
    u32 quad_count;
    OGL_Quad quad_list[OGL_TEXTURE_SIZE_IN_TILES];
    u64 img_list[OGL_TEXTURE_SIZE_IN_TILES];
} OGL;

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
        os_printf("error while compiling the %s shader: %s\n", shader_type, buffer);
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
        os_printf("error while linking shader: %s\n", buffer);
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
    os_printf("[%s] %s\n", prefix, message);
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
    gl->uniform_camera_pos = api->glGetUniformLocation(gl->shader, "camera_pos");
    api->glUseProgram(gl->shader);

    // Setup Vertex Buffer
    v2 verts[] = {
        // Top Left
        {0, 0},
        {1, 1},
        {0, 1},
        // Bottom Right
        {1, 1},
        {0, 0},
        {1, 0},
    };

    api->glBindBuffer(GL_ARRAY_BUFFER, gl->vertex_buffer);
    api->glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STREAM_DRAW);

    api->glEnableVertexAttribArray(0);
    api->glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(v2), (void *)0);

    // Setup Instances
    api->glBindBuffer(GL_ARRAY_BUFFER, gl->instance_buffer);

    OGL_Quad *q0 = 0;
    api->glEnableVertexAttribArray(1);
    api->glVertexAttribDivisor(1, 1);
    api->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(OGL_Quad), (void *)&q0->pos[0]);

    api->glEnableVertexAttribArray(2);
    api->glVertexAttribDivisor(2, 1);
    api->glVertexAttribPointer(2, 3, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(OGL_Quad), (void *)&q0->texture[0]);

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

    // NOTE: We store the images in linear color space!!!
    api->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, OGL_TEXTURE_WIDTH, OGL_TEXTURE_WIDTH, 0, GL_RGBA, GL_FLOAT, 0);

    // Set OpenGL Settings
    api->glEnable(GL_FRAMEBUFFER_SRGB);
    api->glEnable(GL_DEPTH_TEST);
    api->glEnable(GL_MULTISAMPLE);
    api->glEnable(GL_CULL_FACE);
    api->glCullFace(GL_BACK);
    api->glClearColor(.3, .3, .3, 1);
    return gl;
}

static void ogl_begin(OGL *gl) {
    gl->quad_count = 0;
}

static void ogl_quad(OGL *gl, u8 kind, Image *image, v3 pos) {
    if (gl->quad_count >= array_count(gl->quad_list)) {
        os_printf("Too many quads\n");
        return;
    }

    u32 ix = gl->quad_count++;
    u32 tx = ix % OGL_TEXTURE_WIDTH_IN_TILES;
    u32 ty = ix / OGL_TEXTURE_WIDTH_IN_TILES;

    assert(tx < 256, "Out of range");
    assert(ty < 256, "Out of range");

    gl->quad_list[ix] = (OGL_Quad){
        .pos = {pos.x, pos.y, pos.z},
        // tx and ty are not really needed, can just use ix.
        .texture = {tx, ty, kind},
    };

    assert(image->size.x == OGL_TILE_WIDTH && image->size.y == OGL_TILE_WIDTH, "Invalid image size");

    // Copy image
    if (gl->img_list[ix] != image->id) {
        gl->img_list[ix] = image->id;
        gl->api.glTexSubImage2D(GL_TEXTURE_2D, 0, tx * OGL_TILE_WIDTH, ty * OGL_TILE_WIDTH, image->size.x, image->size.y, GL_RGBA, GL_FLOAT, image->pixels);
    }
}

static void ogl_draw(OGL *gl, m4s *mtx, v3 player, v2i viewport_size) {
    OGL_Api *api = &gl->api;

    api->glViewport(0, 0, viewport_size.x, viewport_size.y);
    api->glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    // Setup Instances
    api->glBindBuffer(GL_ARRAY_BUFFER, gl->instance_buffer);
    api->glBufferData(GL_ARRAY_BUFFER, sizeof(OGL_Quad) * gl->quad_count, gl->quad_list, GL_STREAM_DRAW);

    // Screen -> Clip
    api->glUniformMatrix4fv(gl->uniform_proj, 1, false, (GLfloat *)mtx);
    api->glUniform3f(gl->uniform_camera_pos, player.x, player.y, player.z);

    api->glDrawArraysInstanced(GL_TRIANGLES, 0, 6, gl->quad_count);
}
