// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// gl.h - OpenGL 3.3 helper methods
#pragma once
#include "asset.h"
#include "fmt.h"
#include "gl_api.h"
#include "vec.h"

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

typedef struct {
    // Vertex Array Object, Stores bound vertex and index buffers
    GLuint vao;

    // Vertex Buffer Object, Stores vertices
    GLuint vertex_buffer;

    // Element Buffer Object, Stores indicies
    GLuint element_buffer;

    // Shader Program
    GLuint shader;

    // Texture
    GLuint texture;

    bool depth;
} Gl_Pass;

typedef struct {
    Gl_Api api;
    Gl_Pass pass;
} Gl;

static void gl_debug_callback(
    GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message,
    const void *user
) {
    char *prefix = "????";
    if (severity == GL_DEBUG_SEVERITY_HIGH) prefix = "HIGH";
    if (severity == GL_DEBUG_SEVERITY_MEDIUM) prefix = "MEDIUM";
    if (severity == GL_DEBUG_SEVERITY_LOW) prefix = "LOW";
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) prefix = "NOTIFICATION";
    os_printf("[%s] %s\n", prefix, message);
}

static void gl_enable(Gl_Api *gl, GLenum opt, bool value) {
    if (value) {
        gl->glEnable(opt);
    } else {
        gl->glDisable(opt);
    }
}

static void gl_load(Gl *gl, void *load(const char *)) {
    Gl_Api *api = &gl->api;
    Gl_Pass *pass = &gl->pass;

    gl_api_load(api, load);
    api->glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    api->glDebugMessageCallbackARB(gl_debug_callback, 0);

    api->glGenVertexArrays(1, &pass->vao);
    api->glBindVertexArray(pass->vao);

    api->glGenBuffers(1, &pass->vertex_buffer); // vertex buffer object, for vertecies
    pass->shader =
        gl_program_compile_and_link(api, (char *)FILE_SHADER_VERT, (char *)FILE_SHADER_FRAG);
}

static void gl_draw(Gl *gl, v2i viewport_size) {
    Gl_Api *api = &gl->api;
    Gl_Pass *pass = &gl->pass;

    api->glViewport(0, 0, viewport_size.x, viewport_size.y);
    api->glClearColor(.3, .3, .3, 1);
    api->glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    api->glUseProgram(pass->shader);
    api->glBindVertexArray(pass->vao);

    v3 verts[] = {
        {0, 0, 0},
        {1, 0, 0},
        {1, 1, 0},
    };

    api->glBindBuffer(GL_ARRAY_BUFFER, pass->vertex_buffer);
    api->glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STREAM_DRAW);

    api->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(v3), (void *)0);
    api->glEnableVertexAttribArray(0);
    api->glDrawArrays(GL_TRIANGLES, 0, 3);
}
