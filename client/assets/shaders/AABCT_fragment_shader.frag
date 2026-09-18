#version 330 core

in vec2 vTextureCoordinates;

layout(location = 0) out vec4 fragColor;

uniform sampler2D uTexture;
uniform vec3 uColor = vec3(1,1,1);
uniform int uIsThereTexture = 0;

void main()
{
	if(uIsThereTexture == 1)
	{
		fragColor = vec4(uColor,1) * texture(uTexture,vTextureCoordinates);
	}
	else
	{
		fragColor = vec4(uColor,1);
	}
}