#version 430 core

in vec4 v_col;

out vec4 frag_color;

void
main()
{
    frag_color = v_col;
}