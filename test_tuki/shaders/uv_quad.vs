#version 330 core

in vec2 texCoord;

void main()
{
    vec2 pos = 2 * texCoord - vec2(1);
    gl_Position = vec4(pos, 0, 1);
}