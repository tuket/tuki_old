#version 330 core

uniform vec3 color;

in vec3 varPos;
in vec2 varTexCoord;
in vec3 varNormal;

void main()
{
    gl_FragColor = vec4(color, 1);
}