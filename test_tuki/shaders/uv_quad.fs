#version 330 core

uniform vec3 color;

in vec2 p;

void main()
{
    if(p.y > sin(p.x * 10)) gl_FragColor = vec4(color, 1);
    else gl_FragColor = vec4(1-color, 1);
}