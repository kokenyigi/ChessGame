#pragma once
#ifndef UTILS_H
#define UTILS_H

#include <glm/glm.hpp>
#include <string>

struct Plane
{
	float offset;
	glm::vec3 normal;
};


/**
* 
*/
struct Rectangle
{
	float x;
	float y;
	float w;
	float h;
};

struct AABB
{
	glm::vec2 min;
	glm::vec2 max;
};

struct AABB4
{
	glm::vec4 min;
    glm::vec4 max;
};

struct AABB3
{
	glm::vec3 min;
	glm::vec3 max;
};

enum class ValueType
{
	AUTO = 0,
	FIXED = 1,
	RELATIVE = 2
};

struct ValueDefinition
{
	float value = 0.0f;
	ValueType type = ValueType::AUTO;
};

/**
 * Tries to find the intersection of two AABBs.
 * Returns true if they intersect, otherwise exits early with a false retval.
 */
bool IntersectAABB(const AABB& a, const AABB& b, AABB& c);

/**
 * calculates the necessary transforms that transform from -> to
 */
void CalculateRelativeTransformAABB(const AABB& from, const AABB& to, glm::vec2& scale, glm::vec2& offset);

/**
 * Transform an AABB based on scaling and position offset.
 */
void TransformAABB(AABB& a, glm::vec2& scale, glm::vec2& offset);

/**
 * This function is an amalgamation of:
 * - calculating the needed transform that turns start -> end
 * - using this transform on
 */
void TransformBasedOnTwoRelativeAABB(const AABB& start, const AABB& end, AABB& toTransform);

bool IsPointInsideAABB(const AABB& box, const glm::vec2& point);

AABB GetRectangleOfCharacter(char c);

//This functions takes in vec2 raw screen coordinates,
//And spits out the correct NDC coordinates
static glm::vec2 ScreenToNDC(const glm::vec2& rawScreenCoords, int windowWidth, int windowHeight);

static glm::vec2 NDCToScreen(const glm::vec2& normalizedDeviceCoords, int windowWidth, int windowHeight);

//Has numerical stability(division by zero avoidance)
//Calculates u interpolated: p1*(1-u) + p2 * u
static float HorizontalRayToLineIntersect(const glm::vec2& rayOrigin, const glm::vec2& p1, const glm::vec2& p2);

//Returns 0<=t<=1 if ray intersects the line segment between p1 and p2
//Yet to be written
static float RayLineIntersect(const glm::vec2& rayOrigin, const glm::vec2& rayDirection, const glm::vec2& p1, const glm::vec2& p2);

//Turns a positive (0<= integer) into string
static std::string IntegerToString(int integer);

//Return -1 if the string is invalid
static int StringToInteger(const std::string& string);

void GetFileNamesWithSpecificExtension(const std::string& folderPath,
	const std::vector<std::string>& extensions,
	std::vector<std::string>& outputFileNames);



bool TryLoadObjFile(const std::string& filePathRelative,
        std::vector<glm::vec3>& newMeshVertexPositions,
		std::vector<glm::vec3>& newMeshVertexNormals,
		std::vector<glm::vec2>& newMeshTextureCoords,
        std::vector<glm::vec<3,int>>& newMeshTriangleIndices);

AABB4 CalculateAABB4BasedOnTriangles(int startIndex, int endIndex,
	const std::vector<glm::vec3>& positions, 
	const std::vector<glm::vec<3,int>>& triangleIndices);

/**
 * This function calculates an AABB which perfectly encapsulates the transformed AABB given through the parameters
 */
AABB3 GetWorldBoundsOfTransformedAABB(const glm::mat4& worldTransform, const AABB3& localAABB);

void FeedAABB3ToAABB3(AABB3& eater, const AABB3& food);
void FeedVec3ToAABB3(AABB3& eater, const glm::vec3& food);



#endif
