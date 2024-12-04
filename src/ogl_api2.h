// ogl_api2.h: opengl api abstrcation between webgl2 and desktop opengl 3.3
#pragma once

#include "fmt.h"
#include "ogl_api.h"
#include "types.h"

static GLuint _ogl_compile_shader_single(OGL_Api *gl, GLenum type, char *source) {
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

static GLuint ogl_compile_shader(OGL_Api *gl, char *vertex_source, char *fragment_source) {
    GLuint vert_shader = _ogl_compile_shader_single(gl, GL_VERTEX_SHADER, vertex_source);
    GLuint frag_shader = _ogl_compile_shader_single(gl, GL_FRAGMENT_SHADER, fragment_source);
    if (!vert_shader || !frag_shader) return 0;

    // link shaders
    GLuint program = gl->glCreateProgram();
    gl->glAttachShader(program, vert_shader);
    gl->glAttachShader(program, frag_shader);
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

    gl->glDeleteShader(vert_shader);
    gl->glDeleteShader(frag_shader);
    return program;
}

static void _ogl_enable(OGL_Api *gl, GLenum key, bool enable) {
    if (enable) {
        gl->glEnable(key);
    } else {
        gl->glDisable(key);
    }
}

static void ogl_enable_depth_test(OGL_Api *gl, bool enabled) {
    _ogl_enable(gl, GL_DEPTH_TEST, enabled);
}

static void ogl_enable_blend(OGL_Api *gl, bool enabled) {
    _ogl_enable(gl, GL_BLEND, enabled);
    if (enabled) gl->glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
}

static u32 ogl_GenVertexArray(OGL_Api *gl) {
    u32 result = 0;
    gl->glGenVertexArrays(1, &result);
    return result;
}

static void ogl_clear(OGL_Api *gl) {
    gl->glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

static void ogl_TexSubImage2D(OGL_Api *gl, int x, int y, int sx, int sy, void *pixels) {
    gl->glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, sx, sy, GL_RGBA, GL_FLOAT, pixels);
}
