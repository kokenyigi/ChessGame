#version 330 core

in vec3 vPosition;
in vec3 vNormal;

uniform float uBoardWidth;
uniform float uBoardHeight;

uniform vec3 uDarkColor;
uniform vec3 uLightColor;

// this uniform is 2 * 32 bits of flags for each board position
uniform uvec2 uPickingBitmask;
uniform uvec2 uPreviousMoveBitmask;
uniform uvec2 uLegalMovesBitmask;

out vec4 fragColor;

//assumes index is in [0 ,63]
uvec2 CreateBitmaskFromIndex(uint index)
{
    uvec2 retval = uvec2(0u);

    if(index < 32u)
    {
        retval.x = 1u << index;
    }
    else // index is in second coordinate
    {
        retval.y = 1u << (index - 32u);
    }

    return retval;
}

void main()
{
    float halfWidth = uBoardWidth * 0.5f;
    vec2 checkPatternValue = vPosition.xz + vec2(halfWidth,halfWidth);

    float eighthWidth = uBoardWidth * 0.125f;
    ivec2 indexValue = ivec2(checkPatternValue / eighthWidth - 0.0001f);

    indexValue = clamp(indexValue,ivec2(0,0),ivec2(7,7));

    uint indexOfTile = uint(indexValue.x * 8 + indexValue.y);
    uvec2 bitmaskOfTile = CreateBitmaskFromIndex(indexOfTile);
    if(any((bitmaskOfTile & uLegalMovesBitmask) != uvec2(0u)))
    {
        

        vec2 middleOfTile = (vec2(indexValue) + vec2(0.5,0.5)) * vec2(eighthWidth) - vec2(halfWidth);



        vec2 toTileMidFromFrag = middleOfTile - vPosition.xz;

        float radius = eighthWidth * 0.2f;

        if(toTileMidFromFrag.x*toTileMidFromFrag.x + toTileMidFromFrag.y*toTileMidFromFrag.y < radius * radius)
        {
            fragColor = vec4(0,1,0,1);
            return;
        }
    }
    if(any((bitmaskOfTile & uPreviousMoveBitmask) != uvec2(0u)))
    {
        fragColor = vec4(1,1,0,1);
        return;
    }
    
    

    int checkValue = indexValue.y + indexValue.x ;
    if(checkValue % 2 == 0)
    {
        fragColor = vec4(uDarkColor,1);
    }
    else
    {
        fragColor = vec4(uLightColor,1);
    }
    
    
    
}