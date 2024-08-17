// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// gl.h - OpenGL 3.3 helper methods
#pragma once
#include "asset.h"
#include "std.h"
#include "fmt.h"
#include "gl_api.h"
#include "mat.h"
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

    GLuint vertex_buffer;
    GLuint instance_buffer;

    // Shader Program
    GLuint shader;
    GLint uniform_view;
    GLint uniform_proj;
    GLint uniform_camera_pos;

    // Texture
    GLuint texture;

    bool depth;
} Gl_Pass;

typedef struct {
    f32 pos[3];
    u8 texture[3];
} Gl_Quad;

static_assert(sizeof(Gl_Quad) == 16);

typedef struct {
    Gl_Api api;
    Gl_Pass pass;
} Gl;

static void gl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *user) {
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

static Gl *gl_load(Memory *mem, void *load(const char *)) {
    Gl *gl = mem_struct(mem, Gl);
    Gl_Api *api = &gl->api;
    Gl_Pass *pass = &gl->pass;
    gl_api_load(api, load);
    api->glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    api->glDebugMessageCallbackARB(gl_debug_callback, 0);

    api->glGenVertexArrays(1, &pass->vao);
    api->glBindVertexArray(pass->vao);

    api->glGenBuffers(1, &pass->vertex_buffer);
    api->glGenBuffers(1, &pass->instance_buffer);

    pass->shader = gl_program_compile_and_link(api, (char *)FILE_SHADER_VERT, (char *)FILE_SHADER_FRAG);
    pass->uniform_view = api->glGetUniformLocation(pass->shader, "view");
    pass->uniform_proj = api->glGetUniformLocation(pass->shader, "proj");
    pass->uniform_camera_pos = api->glGetUniformLocation(pass->shader, "camera_pos");
    return gl;
}

static void gl_draw(Gl *gl, m4s *mtx, v3 player, v2i viewport_size) {
    Gl_Api *api = &gl->api;
    Gl_Pass *pass = &gl->pass;

    api->glEnable(GL_DEPTH_TEST);

    api->glViewport(0, 0, viewport_size.x, viewport_size.y);
    api->glClearColor(.3, .3, .3, 1);
    api->glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    api->glUseProgram(pass->shader);
    api->glBindVertexArray(pass->vao);

    v2 verts[] = {
        {0, 0},
        {1, 1},
        {0, 1},
        {1, 1},
        {0, 0},
        {1, 0},
    };

    Gl_Quad quads[] = {
        {{0, 0, 0}, { 0, 0, 0 } },
        {{-1, 0, 0}, { 1, 0, 3 } },
        {{1, 0,  0}, { 2, 0,  4 } },
        {{0, 0, -1}, { 3, 0, 2 } },
        {{0, 0, 1}, { 0, 1, 1 } },
        {{0, -1, 0}, { 0, 2, 5 } },
        {{0, 1, 0}, { 0, 3, 6 } },
    };

    // Setup Verts
    api->glBindBuffer(GL_ARRAY_BUFFER, pass->vertex_buffer);
    api->glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STREAM_DRAW);

    api->glEnableVertexAttribArray(0);
    api->glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(v2), (void *)0);

    // Setup Instances
    api->glBindBuffer(GL_ARRAY_BUFFER, pass->instance_buffer);
    api->glBufferData(GL_ARRAY_BUFFER, sizeof(quads), quads, GL_STREAM_DRAW);


    Gl_Quad *q0 = 0;

    api->glEnableVertexAttribArray(1);
    api->glVertexAttribDivisor(1, 1);
    api->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Gl_Quad), (void *)&q0->pos[0]);

    api->glEnableVertexAttribArray(2);
    api->glVertexAttribDivisor(2, 1);
    api->glVertexAttribPointer(2, 3, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(Gl_Quad), (void *)&q0->texture[0]);

    // Current bound buffer does not matter, only VAO matters
    api->glBindBuffer(GL_ARRAY_BUFFER, 0);

    // World -> Screen
    api->glUniformMatrix4fv(pass->uniform_view, 1, false, (GLfloat *)mtx);

    // Screen -> Clip
    m4 proj = m4_id();
    m4_perspective_to_clip(&proj, 70, (f32)viewport_size.x / (f32)viewport_size.y, 0.5, 5.0);
    api->glUniformMatrix4fv(pass->uniform_proj, 1, false, (GLfloat *)&proj.fwd);

    api->glUniform3f(pass->uniform_camera_pos, player.x, player.y, player.z);

    api->glDrawArraysInstanced(GL_TRIANGLES, 0, array_count(verts), array_count(quads));
}
