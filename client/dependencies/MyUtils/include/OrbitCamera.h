#pragma once
#ifndef ORBIT_CAMERA_H
#define ORBIT_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 

class OrbitCamera
{
private:
    float _pitch = 90.0f;
    float _yaw = 0.0f;
    float _roll = 0.0f;

    float _fovx = 45.0f;
    float _fovy = 0.0f; // this is calculated from fovx

    float _aspect = 0.5f; // this is calculated from width / height or vice versa

    float _zNear = 0.1f;
	float _zFar = 1000.0f;

    glm::vec3 _fixatePoint = glm::vec3(0,0,0);
    float _distanceToFixatePoint = 1.0f;
    glm::vec3 _worldUp = glm::vec3(0,1,0);
    glm::vec3 _cameraPosition = glm::vec3(0,1,0);

    float _rotatingSensitivity = 0.25f;
    float _distancingSensitivity = 0.9f;

public:
    //OrbitCamera();

    void Init(const glm::vec3& fixatePoint, const glm::vec3& startPosition, const glm::vec3& worldUp, const glm::vec2& initialDimensions);
    void SetPosition(const glm::vec3& newPos);
    void SetNewFixate(const glm::vec3& newFixate);

    void Resize(const glm::vec2& newDimensions);

    void Rotate(float deltaX, float deltaY);
    void Distance(float deltaValue);

    glm::mat4 GetViewMatrix(){return glm::lookAt(_cameraPosition,_fixatePoint,_worldUp);}
    glm::mat4 GetPerspectiveProjectionMatrix(){return glm::perspective(_fovy,_aspect,_zNear,_zFar);}
    glm::mat4 GetViewXProjectionMatrix(){return  GetPerspectiveProjectionMatrix() * GetViewMatrix();}

private:
    //Calculates the yaw and pitch values based on initial fixate point, worldup and cameraposition
    void CalculateYawAndPitchAndDistance();

    //calcualtes the camera position absed on the fixated point, the distance, the pitch and yaw.
    void CalculatePosition();

    void CalculateFovy();

};

#endif