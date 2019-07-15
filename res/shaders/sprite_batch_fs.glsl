#version 430 core

in vec4 v_tint;
in vec2 v_uv;

out vec4 frag_color;

uniform sampler2D u_sampler;

void
main()
{
    vec4 color = texture(u_sampler, v_uv);
    if (color.a <= 0.0){
        discard;
    }
    frag_color = v_tint * color;
}