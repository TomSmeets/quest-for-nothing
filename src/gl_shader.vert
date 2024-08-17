// Copyright (c) 2024 - Tom Smeets <tom@tsmeets.nl>
// gl_shader.vert - A simple OpenGL vertex Shader
#version 330 core

layout(location = 0) in vec2 vert_uv;
layout(location = 1) in vec3 quad_pos;
layout(location = 2) in vec3 texture;

out vec2 frag_uv;

uniform mat4 view;
uniform mat4 proj;
uniform vec3 camera_pos;

void main() {
    vec2 vert_pos = vert_uv - 0.5;

    vec3 direction = camera_pos - quad_pos;
    float distance = length(direction);
    direction.y = 0;

    // 0 -> sprite
    // 1 -> particle

    vec3 fwd = normalize(direction);
    vec3 rgt = vec3(fwd.z, fwd.y, -fwd.x);
    vec3 up = vec3(0, 1, 0);

    if(texture.z == 1) {
        rgt = vec3(1, 0, 0);
    } else if(texture.z == 2) {
        rgt = vec3(-1, 0, 0);
    } else if(texture.z == 3) {
        rgt = vec3(0, 0, 1);
    } else if(texture.z == 4) {
        rgt = vec3(0, 0, -1);
    } else if(texture.z == 5) {
        rgt = vec3(1, 0, 0);
        up = vec3(0, 0, 1);
    } else if(texture.z == 6) {
        rgt = vec3(-1, 0, 0);
        up = vec3(0, 0, -1);
    }

    
    vec3 pos = quad_pos + rgt * vert_pos.x + up * vert_pos.y;
    frag_uv = (vert_uv + texture.xy) / 4;
    gl_Position = proj * view * vec4(pos, 1);
}
