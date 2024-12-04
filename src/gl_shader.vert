// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// gl_shader.vert - A simple OpenGL vertex Shader
#version 330 core

// Vertex info
// range = [-0.5, 0.5]
layout(location = 0) in vec2 vert_pos;

// Rotation + Scalej
layout(location = 1) in vec3 quad_x;
layout(location = 2) in vec3 quad_y;
layout(location = 3) in vec3 quad_z;

// Translation
layout(location = 4) in vec3 quad_w;

// Texture Altas info
layout(location = 5) in vec2 quad_uv_pos;
layout(location = 6) in vec2 quad_uv_size;

// To Fragment shader
out vec2 frag_uv;
out vec3 frag_normal;
out float frag_z;

uniform mat4 proj;

void main() {
    // Calculate UV in atlas space
    frag_uv = quad_uv_pos + quad_uv_size * .5 + vert_pos * quad_uv_size * vec2(1, -1) * (1.0f - 0.25f / 32.0f);
    // frag_uv = vec2(vert_uv.x, 1- vert_uv.y);
    frag_normal = quad_z;

    // Calculate vertex position (world space)
    vec3 pos = quad_w + vert_pos.x * quad_x + vert_pos.y * quad_y;

    // Project vertex from World -> Clip
    gl_Position = proj * vec4(pos, 1);

    // Pass depth info (-1 to 1)
    frag_z = gl_Position.z;
}
