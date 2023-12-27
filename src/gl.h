// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// gl.h - OpenGL API loader and helper methods
#pragma once
#include "fmt.h"
#include "gl_api.h"
#include "mem.h"
#include "vec.h"
#include "gfx.h"

static void gl_debug_callback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar *message,
    const void *user
) {
    char *prefix = "????";
    switch(severity) {
        case GL_DEBUG_SEVERITY_HIGH:         prefix = "HIGH"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       prefix = "MEDIUM"; break;
        case GL_DEBUG_SEVERITY_LOW:          prefix = "LOW"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: prefix = "NOTIFICATION"; break;
    }
    os_printf("[%s] %s\n", prefix, message);
}

static gl_api *gl_load(mem *m, void *load(const char *)) {
    gl_api *api = mem_struct(m, gl_api);
    gl_api_load(api, load);
    api->glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    api->glDebugMessageCallbackARB(gl_debug_callback, 0);
    return api;
}

static GLuint gl_compile_shader(gl_api *gl, GLenum type, char *source) {
    GLuint shader = gl->glCreateShader(type);
    gl->glShaderSource(shader, 1, (const char *const[]){source}, 0);
    gl->glCompileShader(shader);

    i32 success;
    gl->glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char buffer[1024 * 4];
        gl->glGetShaderInfoLog(shader, sizeof(buffer), 0, buffer);
        char *shader_type = 0;
        if (type == GL_VERTEX_SHADER)   shader_type = "vertex";
        if (type == GL_FRAGMENT_SHADER) shader_type = "fragment";
        os_printf("error while compiling the %s shader: %s\n", shader_type, buffer);
        return 0;
    }

    return shader;
}

static GLuint gl_link_program(gl_api *gl, GLuint vertex, GLuint fragment) {
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
        assert(0);
        return 0;
    }

    return program;
}

static GLuint gl_program_compile_and_link(gl_api *gl, char *vert, char *frag) {
    GLuint vert_shader = gl_compile_shader(gl, GL_VERTEX_SHADER, vert);
    GLuint frag_shader = gl_compile_shader(gl, GL_FRAGMENT_SHADER, frag);
    if (!vert_shader || !frag_shader)
        return 0;

    GLuint shader_program = gl_link_program(gl, vert_shader, frag_shader);
    gl->glDeleteShader(vert_shader);
    gl->glDeleteShader(frag_shader);
    return shader_program;
}

struct gl_t {
    gl_api *api;

    // Vertex Array Object, Stores bound vertex and index buffers
    GLuint vao;

    // Vertex Buffer Object, Stores verticies
    GLuint vbo;

    // Element Buffer Object, Stores indicies
    GLuint ebo;

    // Shader Program
    GLuint shader;

    // Uniform locations
    GLint shader_uniform_img;
    GLint shader_uniform_mat;

    // Texture
    u32 texture_size;
    GLuint texture;
    v4 *empty_image;
};

static gl_t *gl_init(mem *m, gl_api *api) {
    gl_t *gl = mem_struct(m, gl_t);
    gl->api = api;

    // First create and bind the VAO
    api->glGenVertexArrays(1, &gl->vao);
    api->glBindVertexArray(gl->vao);

    api->glGenBuffers(1, &gl->vbo);
    api->glGenBuffers(1, &gl->ebo);

    // load GLSL shader files
    char *vert = (char *)os_read_file(m, "src/gl_shader.vert").ptr;
    char *frag = (char *)os_read_file(m, "src/gl_shader.frag").ptr;
    gl->shader = gl_program_compile_and_link(api, vert, frag);
    gl->shader_uniform_img = api->glGetUniformLocation(gl->shader, "img");
    gl->shader_uniform_mat = api->glGetUniformLocation(gl->shader, "mat");
    assert(gl->shader_uniform_img >= 0);
    assert(gl->shader_uniform_mat >= 0);

    // create a texture atlas where we dynamically write all textures to
    u32 texture_size = 1024;
    gl->texture_size = texture_size;
    api->glGenTextures(1, &gl->texture);
    assert(gl->texture > 0);

    // there is not a good way to clear a texture, so we have to copy an empty texture to the image
    gl->empty_image = mem_array(m, v4, texture_size * texture_size);

    // enable textures, and bind our texture atlas
    api->glActiveTexture(GL_TEXTURE0);
    api->glBindTexture(GL_TEXTURE_2D, gl->texture);
    api->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    api->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    api->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    api->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // These parameters have to be set for the texture.
    // Otherwise we won't see the textures.
    // NOTE: REQUIRED, https://www.khronos.org/opengl/wiki/Common_Mistakes#Creating_a_complete_texture/
    api->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    api->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    // NOTE: We store the images in linear color space!!!
    api->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, texture_size, texture_size, 0, GL_RGBA, GL_FLOAT, gl->empty_image);
    return gl;
}

static void gl_draw(gl_t *gl, v2 window_size) {
    gl_api *api = gl->api;

    // We will use full f32 bit textures and framebuffer
    api->glViewport(0, 0, window_size.x, window_size.y);
    api->glEnable(GL_FRAMEBUFFER_SRGB);
    api->glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    api->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    api->glUseProgram(gl->shader);
    api->glBindVertexArray(gl->vao);
    api->glActiveTexture(GL_TEXTURE0);

    // Clear the texture
    // u32 texture_size = gxl->texture_size;
    // gl->glBindTexture(GL_TEXTURE_2D, gxl->texture);
    // gl->glTexSubImage2D(GL_TEXTURE_2D, 0,
    //     0, 0, texture_size, texture_size,
    //     GL_RGBA, GL_FLOAT,
    //     gxl->empty_image
    // );
}
