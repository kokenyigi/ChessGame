#version 330 core

in vec2 vTextureCoordinates;

layout(location = 0) out vec4 fragColor;

uniform sampler2D uTexture;

void main()
{
	fragColor = texture(uTexture,vTextureCoordinates);
}