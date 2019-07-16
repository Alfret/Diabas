#version 430 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec4 a_col;

out vec4 v_col;

uniform mat4 u_view_proj;

void
main()
{
    v_col = a_col;
    gl_Position = u_view_proj * vec4(a_pos, 1.0);
}