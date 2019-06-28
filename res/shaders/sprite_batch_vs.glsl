#version 430 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec4 a_tint;
layout(location = 2) in vec2 a_uv;

out vec4 v_tint;
out vec2 v_uv;

uniform mat4 u_view_proj;

void
main()
{
    v_tint = a_tint;
    v_uv = a_uv;
    gl_Position = u_view_proj * vec4(a_pos, 1.0);
}