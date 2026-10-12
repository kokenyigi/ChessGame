#version 330 core

in vec3 vPosition;
in vec3 vNormal;

uniform float uBoardWidth;
uniform float uBoardHeight;

uniform vec3 uDarkColor;
uniform vec3 uLightColor;
const vec3 pickingColor = vec3(0,0.7,0);
const vec3 previousMoveColor = vec3(0.8,0.4,0.0);
const vec3 legalMoveColor = vec3(0,1,0);

// this uniform is 2 * 32 bits of flags for each board position
uniform uvec2 uPickingBitmask;
uniform uvec2 uPreviousMoveStart;
uniform uvec2 uPreviousMoveEnd;
uniform uvec2 uLegalMovesBitmask;
uniform uvec2 uLegalCapturesBitmask;

const float legalMoveCircleDiameterRatioToTileWidth = 0.4f;
const float legalCaptureRingInnerDiameterRatioToTileWidth = 0.8f;
const float legalCaptureRingOuterDiameterRatioToTileWidth = 0.9f;

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

    vec2 middleOfTile = (vec2(indexValue) + vec2(0.5,0.5)) * vec2(eighthWidth) - vec2(halfWidth);
    vec2 toTileMidFromFrag = middleOfTile - vPosition.xz;

    uint indexOfTile = uint(indexValue.x * 8 + indexValue.y);
    uvec2 bitmaskOfTile = CreateBitmaskFromIndex(indexOfTile);
    if(any((bitmaskOfTile & uLegalMovesBitmask) != uvec2(0u)))
    {
        float radius = eighthWidth * legalMoveCircleDiameterRatioToTileWidth * 0.5f;
        if(toTileMidFromFrag.x*toTileMidFromFrag.x + toTileMidFromFrag.y*toTileMidFromFrag.y < radius * radius)
        {
            fragColor = vec4(legalMoveColor,1);
            return;
        }
    }
    else if(any((bitmaskOfTile & uLegalCapturesBitmask) != uvec2(0u)))
    {
        float innerRadius = eighthWidth * legalCaptureRingInnerDiameterRatioToTileWidth * 0.5f;
        float innerRadiusSquared = innerRadius * innerRadius;
        
        float outerRadius = eighthWidth * legalCaptureRingOuterDiameterRatioToTileWidth * 0.5f;
        float outerRadiusSquared = outerRadius * outerRadius;

        float compareValue = toTileMidFromFrag.x*toTileMidFromFrag.x + toTileMidFromFrag.y*toTileMidFromFrag.y;
        if(compareValue > innerRadiusSquared && compareValue < outerRadiusSquared)
        {
            fragColor = vec4(legalMoveColor,1);
            return;
        }
    }
    else if(any((bitmaskOfTile & uPickingBitmask) != uvec2(0u)))
    {
        fragColor = vec4(pickingColor,1);
        return;
    }

    if(any((bitmaskOfTile & uPreviousMoveStart) != uvec2(0u)))
    {
        fragColor = vec4(previousMoveColor*vec3(0.8f),1);
        return;
    }
    else if(any((bitmaskOfTile & uPreviousMoveEnd) != uvec2(0u)))
    {
        fragColor = vec4(previousMoveColor,1);
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