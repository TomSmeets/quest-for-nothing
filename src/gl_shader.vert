// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// gl_shader.vert - A simple OpenGL vertex Shader
#version 330 core

layout(location = 0) in vec2 vert_pos;

out vec2 frag_uv;

uniform mat4 mtx;

void main() {
    frag_uv.x = vert_pos.x;
    frag_uv.y = vert_pos.y;

    vec3 pos = vec3(vert_pos * 2 - 1, 0);
    gl_Position = mtx * vec4(pos, 1.0);
}
