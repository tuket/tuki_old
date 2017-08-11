#version 330 core

uniform mat4 modelViewProj;

in vec3 attribPos;
in vec3 attribColor;

out vec3 varColor;

void main()
{
    varColor = attribColor;
    gl_Position = modelViewProj * vec4(attribPos, 1);
}