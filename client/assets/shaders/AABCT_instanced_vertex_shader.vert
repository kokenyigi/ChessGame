#version 330 core

//Vertex variables
layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec2 aTexCoords;

//Instanced variables
layout(location = 2) in vec2 instanceDstOffset;
layout(location = 3) in vec2 instanceDstScale;
layout(location = 4) in vec2 instanceSrcOffset;
layout(location = 5) in vec2 instanceSrcScale;
layout(location = 6) in vec3 instanceColor;
layout(location = 7) in int instanceTextureHandle;
layout(location = 8) in float instanceDepth;

out vec2 vTextureCoordinates;

flat out vec3 flatInstanceColor;
flat out int flatInstanceTextureHandle;

void main()
{
    gl_Position = vec4(instanceDstScale * aPosition + instanceDstOffset,instanceDepth,1.0);
    vTextureCoordinates = instanceSrcScale * aTexCoords + instanceSrcOffset;

    flatInstanceColor = instanceColor;
    flatInstanceTextureHandle = instanceTextureHandle;
}

