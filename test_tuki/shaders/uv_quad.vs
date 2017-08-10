#version 330 core

in vec2 attribTexCoord;

out vec2 p;

void main()
{
    vec2 pos = 2 * attribTexCoord - vec2(1);
    p = pos;
    gl_Position = vec4(pos, 0, 1);
}