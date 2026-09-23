#version 330 core

in vec3 vPosition;
in vec3 vNormal;

uniform vec3 uColor = vec3(1,1,1);

out vec4 fragColor;

void main()
{
    fragColor = vec4(uColor,1);
}