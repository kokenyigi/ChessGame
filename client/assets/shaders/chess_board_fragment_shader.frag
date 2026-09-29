#version 330 core

in vec3 vPosition;
in vec3 vNormal;

uniform float uBoardWidth;
uniform float uBoardHeight;

uniform vec3 uDarkColor;
uniform vec3 uLightColor;

//temp
uniform int uTempIsAPositionPicked;
uniform vec3 uTempPickedPosition;

out vec4 fragColor;

void main()
{
    if(uTempIsAPositionPicked != 0)
    {
        if(length(vPosition - uTempPickedPosition) < 0.3f)
        {
            fragColor = vec4(0,1,0,1);
            return;
        }
    }


    float halfWidth = uBoardWidth * 0.5f;
    vec2 checkPatternValue = vPosition.xz + vec2(halfWidth,halfWidth);

    float eighthWidth = uBoardWidth * 0.125f;
    ivec2 indexValue = ivec2(checkPatternValue / eighthWidth - 0.0001f);

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