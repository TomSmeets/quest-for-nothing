// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// gl_shader.frag - A simple OpenGL fragment Shader
#version 330 core

in vec2 frag_uv;
in vec4 frag_color;

// The final color drawn to the screen
out vec4 out_color;

void main() {
    out_color = frag_color * vec4(frag_uv.xy, 1.0, 1.0);
}
