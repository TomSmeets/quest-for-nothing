// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// gl_shader.frag - A simple OpenGL fragment Shader
#version 330 core

in vec2 frag_uv;

// The final color drawn to the screen
out vec4 frag_color;

void main() {
    frag_color = vec4(frag_uv.x, frag_uv.y, 1.0, 1.0);
}
