// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// gl_shader.vert - A simple OpenGL vertex Shader
#version 330 core

// Vertex attributes, this information is passed directly from the cpu
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_uv;
layout(location = 2) in vec3 a_normal;
layout(location = 3) in vec4 a_color;
layout(location = 4) in float a_emissive;

// Outputs we send to the fragment shader
out vec3 v_uv;
out vec4 v_color;

// Uniforms are passed directly from the cpu per draw call
// This matrix transforms world positions into screen space coordinates
uniform mat4 mat;

void main() {
    // Apply the transformation matrix
    gl_Position = mat * vec4(a_pos, 1.0);

    // directly forward uv coordinates to the fragment shader
    v_uv = a_uv;
    v_color = a_color * (1 + a_emissive);
}
