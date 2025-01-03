// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
// gl_shader.vert - A simple OpenGL vertex Shader
#version 330 core

// Rotation + Scale
layout(location = 0) in vec3 quad_x;
layout(location = 1) in vec3 quad_y;
layout(location = 2) in vec3 quad_z;

// Translation
layout(location = 3) in vec3 quad_w;

// Texture Altas info
layout(location = 4) in vec2 quad_uv_pos;
layout(location = 5) in vec2 quad_uv_size;

// To Fragment shader
out vec2 frag_uv;
out vec3 frag_normal;
out vec3 frag_pos;

uniform mat4 proj;

const vec2 verts[6] = vec2[6](
    // Top Left
    vec2(0, 0), vec2(1, 1), vec2(0, 1),

    // Bottom Right
    vec2(1, 1), vec2(0, 0), vec2(1, 0)
);

void main() {
    vec2 vert_pos = verts[gl_VertexID] - 0.5f;

    // Calculate UV in atlas space
    frag_uv = quad_uv_pos + quad_uv_size * .5 + vert_pos * quad_uv_size * vec2(1, -1) * (1.0f - 0.25f / 32.0f);
    frag_normal = quad_z;

    // Calculate vertex position (world space)
    vec3 pos = quad_w + vert_pos.x * quad_x + vert_pos.y * quad_y;

    // Project vertex from World -> Clip
    gl_Position = proj * vec4(pos, 1);

    // Pass depth info (-1 to 1)
    frag_pos = gl_Position.xyz;
}
