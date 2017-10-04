#version 330 core

uniform mat4 modelViewProjMat;
uniform mat4 modelMat;
uniform mat4 viewProjMat;
uniform mat3 normalMat;

in vec3 attribPos;
in vec3 attribColor;
in vec3 attribTexCoord;
in vec3 attribNormal;
in vec3 attribTangent;
in vec3 attribBitangent;

out vec3 varPos;
out vec2 varTexCoord;
out vec3 varNormal;

void main()
{

    vec4 pos = modelViewProjMat * vec4(attribPos, 1);
    varPos = pos.xyz / pos.w;
    varNormal = normalMat * varNormal;
    varTexCoord = attribTexCoord;

    gl_Position = pos;
}