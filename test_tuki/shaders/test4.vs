#version 330 core

uniform mat4 modelViewProj;

in vec3 attribPos;
in vec2 attribTexCoord;

out vec2 varTexCoord;

void main()
{
    varTexCoord = attribTexCoord;
    gl_Position = modelViewProj * vec4(attribPos, 1);
}