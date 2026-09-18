#version 330 core
        
layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec2 aTextureCoordinates;

uniform vec2 uPosOffset;
uniform vec2 uPosScale;
uniform vec2 uTextureOffset;
uniform vec2 uTextureScale;

out vec2 vTextureCoordinates;

void main()
{
    gl_Position = vec4(uPosScale * aPosition + uPosOffset,0.0,1.0);
    vTextureCoordinates = uTextureScale * aTextureCoordinates + uTextureOffset;
}