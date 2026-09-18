#version 330 core
        
layout(location = 0) in vec2 aPosition;

uniform vec2 uPosOffset;
uniform vec2 uPosScale;

void main()
{
    gl_Position = vec4(uPosScale * aPosition + uPosOffset,0.0,1.0);
}