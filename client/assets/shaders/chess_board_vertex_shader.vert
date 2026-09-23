#version 330 core

in vec3 aPosition;
in vec3 aNormal;

uniform mat4 uViewProjectionMatrix;
uniform mat4 uWorldTransform;
//uniform mat4 

out vec3 vPosition;
out vec3 vNormal;

void main()
{
    gl_Position = uViewProjectionMatrix * uWorldTransform * vec4(aPosition,1.0f);
    vPosition = vec3(uWorldTransform * vec4(aPosition,1));
    vNormal = aNormal; // requires inverse transposed matrix
}