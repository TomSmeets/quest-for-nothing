#pragma once
#include "gfx/ogl_api.h"
#include "lib/fmt.h"
#include "lib/types.h"

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
        fmt_sss(G->fmt, "error while compiling the ", shader_type, " shader: ", buffer, "\n");
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
        fmt_ss(G->fmt, "error while linking shader: ", buffer, "\n");
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

// Prevent spam
static u32 ogl_prev_message_index;
static const char *ogl_prev_message[4];

static void ogl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *user) {
    // Stop spam
    for (u32 i = 0; i < array_count(ogl_prev_message); ++i) {
        if (message == ogl_prev_message[i]) return;
    }

    ogl_prev_message[ogl_prev_message_index] = message;
    ogl_prev_message_index++;
    if (ogl_prev_message_index >= 4) ogl_prev_message_index = 0;

    char *prefix = "????";
    if (severity == GL_DEBUG_SEVERITY_HIGH) prefix = "HIGH";
    if (severity == GL_DEBUG_SEVERITY_MEDIUM) prefix = "MEDIUM";
    if (severity == GL_DEBUG_SEVERITY_LOW) prefix = "LOW";
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) prefix = "NOTIFICATION";
    fmt_sss(G->fmt, "[", prefix, "] ", (char *)message, "\n");
}

static void ogl_enable_debug(OGL_Api *gl) {
    gl->glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    gl->glDebugMessageCallbackARB(ogl_debug_callback, 0);
}
