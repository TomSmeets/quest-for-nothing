// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// gl.h - OpenGL 3.3 helper methods
#pragma once
#include "fmt.h"
#include "gl_api.h"

typedef struct {
    Gl_Api api;
} Gl;

typedef struct {
    // Vertex Array Object, Stores bound vertex and index buffers
    GLuint vao;

    // Vertex Buffer Object, Stores verticies
    GLuint vbo;

    // Element Buffer Object, Stores indicies
    GLuint ebo;

    // Shader Program
    GLuint shader;

    // Texture
    GLuint texture;

    bool depth;
} Gl_Pass;

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

static void gl_load(Gl *gl, void *load(const char *)) {
    gl_api_load(&gl->api, load);
    gl->api.glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    gl->api.glDebugMessageCallbackARB(gl_debug_callback, 0);
}

static void gl_enable(Gl_Api *gl, GLenum opt, bool value) {
    if (value) {
        gl->glEnable(opt);
    } else {
        gl->glDisable(opt);
    }
}

static void gl_draw(Gl *gl, Gl_Pass *pass) {
    Gl_Api *api = &gl->api;
    gl_enable(api, GL_DEPTH_TEST, pass->depth);
    gl_enable(api, GL_BLEND, !pass->depth);

    if (!pass->depth) {
        api->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
}

static void gl_clear(Gl *gl) {
    gl->api.glClearColor(.3, .3, .3, 1);
    gl->api.glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}
