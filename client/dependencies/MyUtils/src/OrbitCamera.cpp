#include "OrbitCamera.h"

#include <iostream>

void OrbitCamera::Init(const glm::vec3 &fixatePoint, const glm::vec3 &startPosition, const glm::vec3 &worldUp, const glm::vec2 &initialDimensions)
{
    this->_fixatePoint = fixatePoint;
    this->_cameraPosition = startPosition;

    Resize(initialDimensions);
    CalculateYawAndPitchAndDistance();
}

void OrbitCamera::Resize(const glm::vec2 &newDimensions)
{
    if(newDimensions .x > 0 && newDimensions.y > 0)
    {
        this->_aspect = newDimensions.x / newDimensions.y;

        CalculateFovy();
    }
}

void OrbitCamera::Rotate(float deltaX, float deltaY)
{
    std::cout<<"Camera Delta: x: " << deltaX << " y: " << deltaY << "\n";

    this->_yaw -= deltaX * _rotatingSensitivity;

    float newPitch = _pitch + deltaY * _rotatingSensitivity;
    if(newPitch < 1.0f)
    {
        newPitch = 1.0f;
    }
    else if(newPitch > 179.0f)
    {
        newPitch = 179.0f;
    }

    this->_pitch = newPitch;

    CalculatePosition();
}

void OrbitCamera::Distance(float deltaValue)
{
    _distanceToFixatePoint *= glm::pow(0.9f, deltaValue *_distancingSensitivity);

    

    CalculatePosition();
}

void OrbitCamera::CalculateYawAndPitchAndDistance()
{
    glm::vec3 diffVec = this->_fixatePoint - this->_cameraPosition;
    float lengthOfDiffVec = glm::length(diffVec);
    glm::vec3 normalizedFront = diffVec / lengthOfDiffVec;

    this->_yaw = glm::degrees(glm::atan(normalizedFront.z,normalizedFront.x));
    this->_pitch = glm::degrees(glm::acos(normalizedFront.y));
    this->_distanceToFixatePoint = lengthOfDiffVec;
}

void OrbitCamera::CalculatePosition()
{
    float inversePitch = 180.0f - _pitch;
    float inverseYaw = -_yaw;

    float inversePitchRad  = glm::radians(inversePitch);
    float inverseYawRad = glm::radians(inverseYaw);

    glm::vec3 diffVec;
    diffVec.x = _distanceToFixatePoint * glm::sin(inversePitchRad) * glm::cos(inverseYawRad);
    diffVec.z = _distanceToFixatePoint * glm::sin(inversePitchRad) * glm::sin(inverseYawRad);
    diffVec.y = _distanceToFixatePoint * glm::cos(inversePitchRad);

    this->_cameraPosition = this->_fixatePoint + diffVec;
}

void OrbitCamera::CalculateFovy()
{
    _fovy = 2.0f*atanf( tanf( glm::radians( _fovx/2.0f ) ) * 1.0f / _aspect);
}
