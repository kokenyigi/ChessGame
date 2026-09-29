#version 330 core

in vec3 vPosition;
in vec3 vNormal;

uniform vec3 uColor = vec3(1,1,1);

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

    fragColor = vec4(uColor,1);
}