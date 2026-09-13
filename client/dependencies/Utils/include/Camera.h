#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // transzform�ci�khoz
#include <glm/gtc/type_ptr.hpp>   

#include <vector>
#include <iostream>

class Camera
{

public:

private:
	float m_yaw = -90.f;
	float m_pitch = 0.0f;
	float m_roll = 0.0f;

	float m_fovy = 0.0f;
	float m_fovx = 45.0f;

	float sensitivity = 0.1f;

	float velocity = 2.5f;
	float speedyVelocity = 10.0f;

	float zNear = 0.1f;
	float zFar = 1000.0f;

	float aspect = 1.0f/2;

	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 m_worldUp;

	glm::vec3 u;
	glm::vec3 v;
	glm::vec3 w;

	bool isSpeedy = false;

	//Gets action inside update(dt) -> this array influences which cardinal direction the cam should move at
	int motionMults[3] = {0,0,0};

public:
	Camera();
	~Camera();

	void Init(const glm::vec3& position, const glm::vec3& at, const glm::vec3& worldUp,
			  int windowWidth,int windowHeight);

	void SetYaw(float yaw);
	float GetYaw() const { return m_yaw; }
	void SetPitch(float pitch);
	float GetPitch() const { return m_pitch; }
	void SetRoll(float roll);
	float GetRoll() const { return m_roll; }

	void SetFovx(float fovx);
	float GetFovx() const { return m_fovx; }

	void SetPosition(const glm::vec3& pos) {this->position = pos;}

	void Resize(int windowWidth, int windowHeight);
	float GetAspect() const { return aspect; }

	float GetZNear()const { return zNear; }
	float GetZFar()const { return zFar; }

	glm::vec3 GetPosition();

	glm::vec3 GetFront()const { return w; }
	glm::vec3 GetUp()const { return v; }
	glm::vec3 GetRight()const { return u; }

	void Update(float deltatime);
	void Rotate(float dx, float dy);

	glm::mat4 GetViewMatrix() const { return glm::lookAt(position, position + front, m_worldUp); }
	glm::mat4 GetPerspectiveMatrix() const 
	{ 
		return glm::perspective(2.0f*atanf( tanf( glm::radians( m_fovx/2.0f ) ) * 1.0f / aspect ), aspect, zNear, zFar); 
	}

	bool GetIsCameraMoving() {return motionMults[0] != 0 || motionMults[1] != 0 || motionMults [2] != 0;}

	//If influence == 0 -> we no longer want to go forward. 
	// if influence == 1 -> we want to move forwards

	//direction == 1 (forward, leftways, upwards)
	// direction == -1 (backwards, rightways, downwards)

	//axisIndex -> 0,1,2 -> x,y,z
	void TryInfluenceMovement(int axisIndex, int direction, int influence);
	//..^^have to calculate fovy from fovx using math

	void SetIsSpeedy(bool isSpeedy){this->isSpeedy = isSpeedy;}
private:
	void CalculateUVW();
	void CalculateFront();
};

#endif