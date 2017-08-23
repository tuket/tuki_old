#version 330 core

uniform sampler2D colorTex;

in vec2 varTexCoord;

void main()
{
    vec2 tc = varTexCoord;
    gl_FragColor = texture(colorTex, tc);
}