// Copyright (c) 2025 - Tom Smeets <tom@tsmeets.nl>
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
    // Texture
    out_color = texture(img, frag_uv);

    // Distance fog
    float z_near = 0.1;
    float z_far = 15.0;
    float z_rel = (frag_pos.z - z_near) / (z_far - z_near);
    out_color.rgb = mix(out_color.rgb, vec3(0.02f), clamp(z_rel, 0, 1));
}
