// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// gl_shader.vert - A simple OpenGL vertex Shader
#version 330 core

layout(location = 0) in vec2 vert_uv;

layout(location = 1) in vec3 quad_x;
layout(location = 2) in vec3 quad_y;
layout(location = 3) in vec3 quad_z;
layout(location = 4) in vec3 quad_w;
layout(location = 5) in vec2 quad_uv_pos;
layout(location = 6) in vec2 quad_uv_size;

out vec2 frag_uv;
out vec3 frag_normal;
out float frag_z;

uniform mat4 proj;
uniform vec3 camera_pos;

void main() {
    vec2 vert_pos = vert_uv - 0.5;

    frag_uv = quad_uv_pos - vec2(vert_uv.x, vert_uv.y) * quad_uv_size + quad_uv_size;
    frag_normal = quad_z;

    vec3 pos = quad_w + vert_pos.x * quad_x + vert_pos.y * quad_y;

    // 0,0 is top left
    gl_Position = proj * vec4(pos, 1);
    frag_z = gl_Position.z;

    // Map Vulkan to OpenGL coordnaites
    // gl_Position.y *= -1;
    // gl_Position.z = (gl_Position.z + gl_Position.w) / 2.0f;
}
