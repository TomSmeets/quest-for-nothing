// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// gl_shader.vert - A simple OpenGL vertex Shader
#version 330 core

layout(location = 0) in vec3 vert_pos;

void main() {
    gl_Position = vec4(vert_pos, 1.0);
}
