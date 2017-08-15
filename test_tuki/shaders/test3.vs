#version 330 core

in vec2 attribTexCoord;

out vec2 varTexCoord;

void main()
{
    varTexCoord = attribTexCoord;
    vec2 pos = 2 * attribTexCoord - vec2(1);
    gl_Position = vec4(pos, 0, 1);
}