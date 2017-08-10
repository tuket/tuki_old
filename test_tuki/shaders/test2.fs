#version 330 core

in vec3 varColor;

void main()
{
    gl_FragColor = vec4(varColor, 1);
}