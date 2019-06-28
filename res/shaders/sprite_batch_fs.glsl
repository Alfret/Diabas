#version 430 core

in vec4 v_tint;
in vec2 v_uv;

out vec4 frag_color;

uniform sampler2D u_sampler;

void
main()
{
    frag_color = v_tint * texture(u_sampler, v_uv);
}