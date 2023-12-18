// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// gl_shader.vert - A simple OpenGL vertex Shader
#version 330 core

// Vertex attributes, this information is passed directly from the cpu
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec2 a_uv;

// Outputs we send to the fragment shader
out vec2 v_uv;

// Uniforms are passed directly from the cpu per draw call
// This matrix transforms world positions into screen space coordinates
uniform mat4 mat;

void main() {
    // Apply the transformation matrix
    gl_Position = mat * vec4(a_pos, 1.0);

    // directly forward uv coordinates to the fragment shader
    v_uv = a_uv;
}
