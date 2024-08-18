// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// gl.h - OpenGL 3.3 helper methods
#pragma once
#include "asset.h"
#include "fmt.h"
#include "gl_api.h"
#include "image.h"
#include "mat.h"
#include "std.h"
#include "vec.h"

typedef struct {
    f32 pos[3];
    u8 texture[3];
} Gl_Quad;

static_assert(sizeof(Gl_Quad) == 16);

typedef struct {
    Gl_Api api;

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

    u32 quad_count;
    Gl_Quad quad_list[1024];
    u64 img_list[1024];
} Gl;

static u32 gl_prev_message_index;
static const char *gl_prev_message[4];

static GLuint gl_compile_shader(Gl_Api *gl, GLenum type, char *source) {
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

static GLuint gl_link_program(Gl_Api *gl, GLuint vertex, GLuint fragment) {
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

static GLuint gl_program_compile_and_link(Gl_Api *gl, char *vert, char *frag) {
    GLuint vert_shader = gl_compile_shader(gl, GL_VERTEX_SHADER, vert);
    GLuint frag_shader = gl_compile_shader(gl, GL_FRAGMENT_SHADER, frag);
    if (!vert_shader || !frag_shader) return 0;

    GLuint shader_program = gl_link_program(gl, vert_shader, frag_shader);
    gl->glDeleteShader(vert_shader);
    gl->glDeleteShader(frag_shader);
    return shader_program;
}

static void gl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *user) {
    // Stop spam
    for (u32 i = 0; i < array_count(gl_prev_message); ++i) {
        if (message == gl_prev_message[i]) return;
    }

    char *prefix = "????";
    if (severity == GL_DEBUG_SEVERITY_HIGH) prefix = "HIGH";
    if (severity == GL_DEBUG_SEVERITY_MEDIUM) prefix = "MEDIUM";
    if (severity == GL_DEBUG_SEVERITY_LOW) prefix = "LOW";
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) prefix = "NOTIFICATION";
    os_printf("[%s] %s\n", prefix, message);
    gl_prev_message[gl_prev_message_index] = message;
    gl_prev_message_index++;
    if (gl_prev_message_index >= 4) gl_prev_message_index = 0;
}

static Gl *gl_load(Memory *mem, void *load(const char *)) {
    Gl *gl = mem_struct(mem, Gl);
    Gl_Api *api = &gl->api;

    // Load OpenGL function pointers
    gl_api_load(api, load);

    // Debug Output
    api->glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    api->glDebugMessageCallbackARB(gl_debug_callback, 0);

    // Create VAO
    api->glGenVertexArrays(1, &gl->vao);
    api->glBindVertexArray(gl->vao);

    api->glGenBuffers(1, &gl->vertex_buffer);
    api->glGenBuffers(1, &gl->instance_buffer);

    // Compile Shader
    gl->shader = gl_program_compile_and_link(api, (char *)FILE_SHADER_VERT, (char *)FILE_SHADER_FRAG);
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

    Gl_Quad *q0 = 0;
    api->glEnableVertexAttribArray(1);
    api->glVertexAttribDivisor(1, 1);
    api->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Gl_Quad), (void *)&q0->pos[0]);

    api->glEnableVertexAttribArray(2);
    api->glVertexAttribDivisor(2, 1);
    api->glVertexAttribPointer(2, 3, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(Gl_Quad), (void *)&q0->texture[0]);

    // Texture atlas
    u32 tile_count = 32;
    u32 tile_size = 32;

    u32 texture_size = tile_count * tile_size;
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
    api->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, texture_size, texture_size, 0, GL_RGBA, GL_FLOAT, 0);

    // Set OpenGL Settings
    api->glEnable(GL_FRAMEBUFFER_SRGB);
    api->glEnable(GL_DEPTH_TEST);
    api->glEnable(GL_MULTISAMPLE);
    api->glEnable(GL_CULL_FACE);
    api->glCullFace(GL_BACK);
    api->glClearColor(.3, .3, .3, 1);
    return gl;
}

static void gl_begin(Gl *gl) {
    gl->quad_count = 0;
}

static void gl_quad(Gl *gl, u8 kind, Image *image, v3 pos) {
    if (gl->quad_count >= array_count(gl->quad_list)) {
        os_printf("Too many quads\n");
        return;
    }

    u32 ix = gl->quad_count++;

    u32 tx = ix % 32;
    u32 ty = ix / 32;

    gl->quad_list[ix] = (Gl_Quad){
        .pos = {pos.x, pos.y, pos.z},
        // tx and ty are not really needed, can just use ix.
        .texture = {tx, ty, kind},
    };

    assert(image->size.x == 32 && image->size.y == 32, "Invalid image size");

    // Copy image
    if (gl->img_list[ix] != image->id) {
        gl->img_list[ix] = image->id;
        gl->api.glTexSubImage2D(GL_TEXTURE_2D, 0, tx * 32, ty * 32, image->size.x, image->size.y, GL_RGBA, GL_FLOAT, image->pixels);
    }
}

static void gl_draw(Gl *gl, m4s *mtx, v3 player, v2i viewport_size) {
    Gl_Api *api = &gl->api;

    api->glViewport(0, 0, viewport_size.x, viewport_size.y);
    api->glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    // Setup Instances
    api->glBindBuffer(GL_ARRAY_BUFFER, gl->instance_buffer);
    api->glBufferData(GL_ARRAY_BUFFER, sizeof(Gl_Quad) * gl->quad_count, gl->quad_list, GL_STREAM_DRAW);

    // Screen -> Clip
    api->glUniformMatrix4fv(gl->uniform_proj, 1, false, (GLfloat *)mtx);
    api->glUniform3f(gl->uniform_camera_pos, player.x, player.y, player.z);

    api->glDrawArraysInstanced(GL_TRIANGLES, 0, 6, gl->quad_count);
}
