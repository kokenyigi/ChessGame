#version 330 core

layout(location = 0) out vec4 fragColor;

uniform vec3 uColor;

void main()
{
	fragColor = vec4(uColor,1.0f);
}