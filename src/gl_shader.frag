// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// gl_shader.frag - A simple OpenGL fragment Shader
#version 330 core

in vec2 frag_uv;

// The final color drawn to the screen
out vec4 out_color;

// The texture atlas we can sample from
uniform sampler2D img;

void main() {
    // Taken from: Crafting a Better Shader for Pixel Art Upscaling
    // https://www.youtube.com/watch?v=d6tp43wZqps&t=30s
    vec2 tex_size = vec2(1024);
    vec2 box_size = min(fwidth(frag_uv) * tex_size, 1);
    vec2 center = (floor(frag_uv * 32) + 0.5) / 32;
    vec2 tx = frag_uv * tex_size - 0.5 * box_size;
    vec2 tx_offset = smoothstep(1 - box_size, vec2(1), fract(tx));
    vec2 uv = (floor(tx) + 0.5 + tx_offset) / tex_size;
    out_color = textureGrad(img, uv, dFdx(frag_uv), dFdy(frag_uv));
}
