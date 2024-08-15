// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// gl_shader.frag - A simple OpenGL fragment Shader
#version 330 core

// The final color drawn to the screen
out vec4 frag_color;

void main() {
    frag_color = vec4(1.0, 0.0, 1.0, 1.0);
}
