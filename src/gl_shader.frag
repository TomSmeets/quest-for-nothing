// Copyright (c) 2023 - Tom Smeets <tom@tsmeets.nl>
// gl_shader.frag - A simple OpenGL fragment Shader
#version 330 core

// The final color drawn to the screen
out vec4 out_color;

// The texture atlas we can sample from
uniform sampler2D img;

// Interpolated values sent from the vertex shader
in vec2 v_uv;
in float v_color;

void main() {
    // Lookup the color in the texture atlas
    out_color = texture(img, v_uv);

    // Limit the color to [0-1]
    // All colors are in [0-inf] range, this allows us to have very bright lights
    // In the future we want to do some HDR stuff such as bloom.
    // out_color.rgb /= max(length(out_color.rgb) / length(vec3(1,1,1)), 1);

    // out_color = vec4(vec3(v_uv, 1)*.5 + out_color.rgb*.5, 1);

    // Discard transparent pixels
    if (out_color.a <= 0.1)
        discard;
}
