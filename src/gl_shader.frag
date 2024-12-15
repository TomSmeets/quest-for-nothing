// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// gl_shader.frag - A simple OpenGL fragment Shader
#version 330 core

in vec2 frag_uv;
in vec3 frag_normal;
in vec3 frag_pos;

// The final color drawn to the screen
out vec4 out_color;

// The texture atlas we can sample from
uniform sampler2D img;

void main() {
    vec2 texture_size = vec2(2048 * 2);

    // Taken from: Crafting a Better Shader for Pixel Art Upscaling
    // https://www.youtube.com/watch?v=d6tp43wZqps&t=30s
    vec2 box_size = min(fwidth(frag_uv) * texture_size, 1);
    vec2 tx = frag_uv * texture_size - 0.5 * box_size;
    vec2 tx_offset = smoothstep(1 - box_size, vec2(1), fract(tx));
    vec2 uv = (floor(tx) + 0.5 + tx_offset) / texture_size;

    // vec3 normal = normalize(frag_normal);
    out_color = textureGrad(img, uv, dFdx(frag_uv), dFdy(frag_uv));

    // if(frag_z > 3) out_color.rgb = vec3(1, 0, 0);
    // Distance fog
    float z_near = 0.1;
    float z_far = 15.0;
    float z_rel = (frag_pos.z - z_near) / (z_far - z_near);
    out_color.rgb = mix(out_color.rgb, vec3(0.02f), clamp(z_rel, 0, 1));
    // out_color = texture(img, frag_uv);
    // out_color = vec4(frag_uv, 1, out_color.a);
    if (out_color.a < 0.5) discard;
}
