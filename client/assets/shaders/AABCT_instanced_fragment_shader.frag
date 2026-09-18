#version 330 core

in vec2 vTextureCoordinates;

flat in vec3 flatInstanceColor;
flat in int flatInstanceTextureHandle;

uniform samplerBuffer uTexelBuffer;
uniform usamplerBuffer uTextureRegistryBuffer;


out vec4 fragColor;

void main()
{
    if(flatInstanceTextureHandle < 0)
    {
        fragColor = vec4(flatInstanceColor,1);
    }
    else
    {
        //lets texture
        uvec4 textureRegistryEntry = texelFetch(uTextureRegistryBuffer,flatInstanceTextureHandle);

        uint startIndex = textureRegistryEntry.x;
        uint width = textureRegistryEntry.y;
        uint height = textureRegistryEntry.z;

        uint x = min(uint(vTextureCoordinates.x * float(width)), width - 1u);

        uint y = min(uint(vTextureCoordinates.y * float(height)), height - 1u);

        uint textureIndex = startIndex + width * x + y;

        vec4 texel = texelFetch(uTexelBuffer, textureIndex);

        if(texel.a < 0.001)
        {
            discard;
        }

        fragColor = texel * vec4(flatInstanceColor,1);
    }
}