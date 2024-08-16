// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// gl_shader.vert - A simple OpenGL vertex Shader
#version 330 core

layout(location = 0) in vec2 vert_uv;
layout(location = 1) in vec3 quad_pos;

out vec2 frag_uv;
out vec4 frag_color;

uniform mat4 view;
uniform mat4 proj;
uniform vec3 camera_pos;

void main() {
    vec2 vert_pos = vert_uv - vec2(0.5, 0);

    vec3 direction = camera_pos - quad_pos;
    float distance = length(direction);

    vec3 fwd = normalize(direction * vec3(1, 0, 1));
    vec3 rgt = vec3(fwd.z, 0, -fwd.x);
    vec3 up = vec3(0, 1, 0);

    vec3 pos = quad_pos + rgt * vert_pos.x + up * vert_pos.y;

    frag_uv = vert_uv;
    frag_color = vec4(1, 1, 1, 1);
    gl_Position = proj * view * vec4(pos, 1);
}
