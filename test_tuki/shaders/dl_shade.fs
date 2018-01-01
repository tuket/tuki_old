#version 330 core

uniform vec3 color;
uniform vec3 L;

in vec3 varPos;
in vec2 varTexCoord;
in vec3 varNormal;

void main()
{
	vec3 N = varNormal;
	float intensity = dot(N, L);
    gl_FragColor = vec4(intensity * color, 1);
}