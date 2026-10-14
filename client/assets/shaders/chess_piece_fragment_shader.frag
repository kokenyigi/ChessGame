#version 330 core

in vec3 vPosition;
in vec3 vNormal;

uniform vec3 uColor;

const vec3 lightDirection = -normalize(vec3(1,1,1));
const vec3 ambientColor = vec3(0.4,0.4,0.4);
const vec3 diffuseColor = vec3(0.6,0.6,0.6);

out vec4 fragColor;

void main()
{
    vec3 fragNormal = normalize(vNormal);

    float diffuseStrength = max(dot(fragNormal,-lightDirection),0);
    vec3 diffuseComponent = diffuseColor * diffuseStrength;

    vec3 lighting = ambientColor + diffuseComponent;


    //uColor * lighting
    fragColor = vec4(uColor * lighting,1);
}