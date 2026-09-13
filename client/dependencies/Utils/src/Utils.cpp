#include "Utils.h"

#include <iostream>

#include <vector>
#include <algorithm>
#include <filesystem>

#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>


bool IntersectAABB(const AABB& a, const AABB& b, AABB& c)
{
	if(a.min.x < b.max.x && a.max.x > b.min.x && a.min.y < b.max.y && a.max.y > b.min.y)
	{
		c.min.x = fmaxf(a.min.x, b.min.x);
		c.min.y = fmaxf(a.min.y, b.min.y);
		c.max.x = fminf(a.max.x, b.max.x);
		c.max.y = fminf(a.max.y, b.max.y);
		return true;
	}
	else
	{
		c = {glm::vec2(0,0),glm::vec2(0,0)};
	}

	return false;
}

/**
 * calculates the necessary transforms that transform from -> to
 */
void CalculateRelativeTransformAABB(const AABB& from, const AABB& to, glm::vec2& scale, glm::vec2& offset)
{
	float fromW = (from.max.x - from.min.x);
	float fromH = (from.max.y - from.min.y);
	float toW = (to.max.x - to.min.x);
	float toH = (to.max.y - to.min.y);

	scale.x =  toW / fromW;
	scale.y = toH / fromH;

	offset.x = (to.min.x - from.min.x) / fromW;
	offset.y = (to.min.y - from.min.y) / fromH;
}

/**
 * Transform an AABB based on scaling and position offset.
 */
void TransformAABB(AABB& a, glm::vec2& scale, glm::vec2& offset)
{
	float aW = (a.max.x - a.min.x);
	float aH = (a.max.y - a.min.y);

	float aScaledW = aW * scale.x;
	float aScaledH = aH * scale.y;

	float newMinX = a.min.x + offset.x * aW;
	float newMinY = a.min.y + offset.y * aH;

	a.min.x = newMinX;
	a.min.y  = newMinY;
	a.max.x = newMinX + aScaledW;
	a.max.y = newMinY + aScaledH;
}

/**
 * This function is an amalgamation of:
 * - calculating the needed transform that turns start -> end
 * - using this transform on
 */
void TransformBasedOnTwoRelativeAABB(const AABB& start, const AABB& end, AABB& toTransform)
{
	glm::vec2 scale;
	glm::vec2 offset;
	CalculateRelativeTransformAABB(start,end,scale,offset);
	TransformAABB(toTransform,scale,offset);
}

bool IsPointInsideAABB(const AABB &box, const glm::vec2 &point)
{
	return point.x >= box.min.x && point.x <= box.max.x && point.y >= box.min.y && point.y <= box.max.y;
}

AABB GetRectangleOfCharacter(char c)
{
	AABB retval = {glm::vec2(0.0f,0.0f),glm::vec2(0.1f,0.1f)};

    if(c >= ' ' && c<= '~')
    {    
        if(c != 'M' && c!= 'W' && c!= 'w' && c!= 'm')
        {
            int charDiff = c - ' ' - (char)1;

            float x = 0.1f * (charDiff % 10);
            float y = 0.1f * (charDiff / 10);

            retval.min = glm::vec2(x,y);
            retval.max = retval.min + glm::vec2(0.1f,0.1f);
        }
        else
        {
            if(c== 'M')
            {
                retval.min=glm::vec2(0.4f,0.9f);
                retval.max = retval.min + glm::vec2(0.15f,0.1f);
            }
            else if(c == 'W')
            {
                retval.min=glm::vec2(0.55f,0.9f);
                retval.max = retval.min + glm::vec2(0.15f,0.1f);
            }
            else if(c == 'm')
            {
                retval.min=glm::vec2(0.70f,0.9f);
                retval.max = retval.min + glm::vec2(0.15f,0.1f);
            }
            else if(c == 'w')
            {
                retval.min=glm::vec2(0.85f,0.9f);
                retval.max = retval.min + glm::vec2(0.15f,0.1f);
            }
        }
    }

    return retval;
}

//This functions takes in vec2 raw screen coordinates,
//And spits out the correct NDC coordinates
static glm::vec2 ScreenToNDC(const glm::vec2& rawScreenCoords, int windowWidth, int windowHeight)
{
	float normalizedDeviceX = (rawScreenCoords.x / (float)windowWidth) * 2 - 1;
	float normalizedDeviceY = ((rawScreenCoords.y / (float)windowHeight) * 2 - 1) * -1;

	return glm::vec2(normalizedDeviceX, normalizedDeviceY);
}

static glm::vec2 NDCToScreen(const glm::vec2& normalizedDeviceCoords, int windowWidth, int windowHeight)
{
	float screenX = (normalizedDeviceCoords.x + 1.0f) / 2.0f * windowWidth;
	float screenY = (-normalizedDeviceCoords.y + 1.0f) / 2.0f * windowHeight;

	return glm::vec2(screenX, screenY);
}

//Has numerical stability(division by zero avoidance)
//Calculates u interpolated: p1*(1-u) + p2 * u
static float HorizontalRayToLineIntersect(const glm::vec2& rayOrigin, const glm::vec2& p1, const glm::vec2& p2)
{
	//we know our ray's direction is: vec2(1,0)

	//If this happens we know that the line segment is almost paralell to our ray
	//Meaning no collision, therefore we return -1
	if (fabsf(p1.y - p2.y) < 0.0000001f)
	{
		return -1;
	}

	float u = (rayOrigin.y - p1.y) / (p2.y - p1.y);
	if (u >= 0 && u <= 1)
	{
		float t = p1.x - rayOrigin.x + u * (p2.x - p1.x);
		return t;
	}
	
	return -1;
	
}

//Returns 0<=t<=1 if ray intersects the line segment between p1 and p2
//Yet to be written
static float RayLineIntersect(const glm::vec2& rayOrigin, const glm::vec2& rayDirection, const glm::vec2& p1, const glm::vec2& p2)
{
	return -1.0f;
}

//Turns a positive (0<= integer) into string
static std::string IntegerToString(int integer)
{
	if (integer < 0)
	{
		return "";
	}
	if (integer == 0)
	{
		return "0";
	}
	char zeroAscii = '0';
	std::string retval = "";
	int number = integer;
	while (number > 0)
	{
		int currentNumber = number % 10;
		number /= 10;

		retval = (char)(zeroAscii + currentNumber) + retval;
	}

	


	return retval;
}

//Return -1 if the string is invalid
static int StringToInteger(const std::string& string)
{
	if (string.size() < 1)
	{
		return -1;
	}
	if (string.size() == 1 && string[0] == '0')
	{
		return 0;
	}

	if (string[0] == '0')
	{
		//cant start with 0
		return -1;
	}
	
	char zeroAscii = '0';
	int retval = 0;
	for (int i = 0;i < string.size();++i)
	{
		if (string[i] > '9' || string[i] < '0')
		{
			return -1;
		}
		retval = retval * 10 + (int)(string[i] - zeroAscii);
	}
		
	return retval;

}

namespace fs = std::filesystem;

void GetFileNamesWithSpecificExtension(const std::string &folderPath, const std::vector<std::string> &extensions, std::vector<std::string> &outputFileNames)
{
	outputFileNames.clear();

	for(const auto& directoryEntry : fs::directory_iterator(folderPath))
	{
		if(directoryEntry.is_regular_file())
		{
			std::string extensionOfFile = directoryEntry.path().extension().string();

			if(std::find(extensions.begin(),extensions.end(),extensionOfFile) != extensions.end())
			{
				//we found a good file, so we save it
				outputFileNames.push_back(directoryEntry.path().filename().string());
			}
		}
	}
}

bool TryLoadObjFile(const std::string &filePathRelative, 
	std::vector<glm::vec3> &newMeshVertexPositions, 
	std::vector<glm::vec3> &newMeshVertexNormals, 
	std::vector<glm::vec2> &newMeshTextureCoords, 
	std::vector<glm::vec<3, int>> &newMeshTriangleIndices)	
{
    //De-initialize parameter vectors
    newMeshVertexPositions.clear();
    newMeshVertexNormals.clear();
	newMeshTextureCoords.clear();
    newMeshTriangleIndices.clear();

    //Setup vectors that contain the loaded mesh data
    std::vector<glm::vec3> uniqueVertexPositions;
    std::vector<glm::vec3> uniqueVertexNormals;
    std::vector<glm::vec2> uniqueVertexTexCoords;

    // Now we prepare our unique-keeping system, which will be a hash-set(unordered)
    // This struct contains the components which define a unique VERTEX, 
    struct IndexKey
    {
        int positionIndex = -1;
        int normalIndex = -1;
        int textureIndex = -1;

        bool operator==(const IndexKey& other) const
        {
            return positionIndex == other.positionIndex &&
                normalIndex == other.normalIndex &&
                textureIndex == other.textureIndex;
        }
    };
    struct IndexKeyHash
    {
        std::size_t operator()(const IndexKey& k) const
        {
            return ((std::hash<int>()(k.positionIndex) ^
                (std::hash<int>()(k.normalIndex) << 1)) >> 1) ^
                (std::hash<int>()(k.textureIndex) << 1);
        }
    };

    /**
     * The goal: eliminating duplicate vertices
     * A vertex is defined by its vertex position index, normal index and texcoords index
     * If the indexing would want us to make a new vertex, whichs three index components(see above)
     * Already exist inside this map, then we use the stored, already created index value instead of creating a new one.
     */
    std::unordered_map<IndexKey, unsigned int, IndexKeyHash> threeIndexComponentToVertexIndex;

    //These boolean values are set to true if the mesh has the given properties
    bool doesMeshHaveTexcoords = false;
    bool doesMeshHaveNormals = false;

    std::ifstream meshFile(filePathRelative);
    std::string meshFileLineBuffer;
    while(std::getline(meshFile,meshFileLineBuffer)) // Here we read the meshfile till no more row exist
    {
        //We now acquire the words of the line.
        std::stringstream lineStringStream(meshFileLineBuffer);
        std::vector<std::string> wordsOfLine;
        std::string currentWord;
        while(lineStringStream >> currentWord)
        {
            wordsOfLine.push_back(currentWord);
        }

        //Lets parse the given words.
        if(wordsOfLine.size() <= 1) continue;

        //We have found a vertex position, lets add it to the unique positions
        if(wordsOfLine[0] == "v")
        {
            //Safe early exit, in case the file doesnt have 3d vertices or its corrupted.
            if(wordsOfLine.size() < 4) return false;
            
            glm::vec3 newVertexPosition;
            newVertexPosition.x = std::stof(wordsOfLine[1]);
            newVertexPosition.y = std::stof(wordsOfLine[2]);
            newVertexPosition.z = std::stof(wordsOfLine[3]);

            uniqueVertexPositions.push_back(newVertexPosition);
        }
        else if(wordsOfLine[0] == "vt")
        {
            if(wordsOfLine.size() < 3) return false;

            if(!doesMeshHaveTexcoords)
            {
                doesMeshHaveTexcoords = true;
            }

            glm::vec2 newVertexTexCoords;
            newVertexTexCoords.x = std::stof(wordsOfLine[1]);
            newVertexTexCoords.y = std::stof(wordsOfLine[2]);

            uniqueVertexTexCoords.push_back(newVertexTexCoords);
        }
        else if(wordsOfLine[0] == "vn")
        {
            if(wordsOfLine.size() < 4) return false;

            if(!doesMeshHaveNormals)
            {
                doesMeshHaveNormals = true;
            }

            glm::vec3 newVertexNormal;
            newVertexNormal.x = std::stof(wordsOfLine[1]);
            newVertexNormal.y = std::stof(wordsOfLine[2]);
            newVertexNormal.z = std::stof(wordsOfLine[3]);

            uniqueVertexNormals.push_back(newVertexNormal);
        }
        else if(wordsOfLine[0] == "f")
        {
            //Now we have to assemble the vertices, and form indices with them
            /**
             * DISCLAIMER! there are more than one ways an .obj file can store its mesh:
             *  - has positions,texcoords,normals -> easy stuff
             *  - has positions,texcoords (doesn't have normals) -> we do the same, but here we have to calculate the normals
             *  - has positions, (doesnt have texcoords), normals -> lets just simply replace the texcoords with -1,-1 for now.
             *  - has positions (and doesnt fucking have anything else) -> we calculate normals, and replace texcoords with -1,-1.
             * 
             * One more thing: when indexing, a face can consist of undefined numbers of vertices (triangles, quads ..etc.)
             * for this reason, we have to assemble the vertices in a weird, polygon-wise manner.
             */

            if(wordsOfLine.size() < 4 ) return false;

            int polygonSize = wordsOfLine.size() -1 ;
            std::vector<unsigned int> polygonVertexIndices;
            polygonVertexIndices.reserve(polygonSize);

			int newlyCreatedVertexCount = 0;

			//We construct/pull the specified polygon vertices
			for(int i=0;i<polygonSize;++i)
			{
				std::stringstream currentVertexSpecification(wordsOfLine[i+1]);
				std::vector<std::string> currentVertexAttributeIndices;
				std::string buffer;
				while(std::getline(currentVertexSpecification,buffer,'/'))
				{
					currentVertexAttributeIndices.push_back(buffer);
				}

				IndexKey indexKey;

				//We are bound to have a position.
				indexKey.positionIndex = std::stoi(currentVertexAttributeIndices[0]);

				//Now we have to figure out if we have a texture coord specified.
				if(currentVertexAttributeIndices.size() >= 2 && currentVertexAttributeIndices[1].length() != 0)
				{
					indexKey.textureIndex = std::stoi(currentVertexAttributeIndices[1]);
				}

				//If the length of the read index strings is larger or equal to 3 we must have a normal defined.
				if(currentVertexAttributeIndices.size() >= 3)
				{
					indexKey.normalIndex = std::stoi(currentVertexAttributeIndices[2]);
				}


				if(doesMeshHaveNormals)
				{
					
				}
				//Now that we have parsed the read index set of p - t - n , then now we check if these combinations of vertices
				//Already exists inside the hasmap, if yes we just simply use the index stored inside there.
				auto mapIt = threeIndexComponentToVertexIndex.find(indexKey);
				
				if (mapIt != threeIndexComponentToVertexIndex.end() && doesMeshHaveNormals)
				{
					//we found the key, therefore we already have a vertex specified with these indices
					unsigned int vertexIndex = mapIt->second;
					polygonVertexIndices.push_back(vertexIndex);
				}
				else
				{
					//If we are here, we havent found a vertex with these p - t - n index combinations.
					//Therefore, we must CREATE a new vertex.
					int effectivePositionIndex = indexKey.positionIndex < 0 ? uniqueVertexPositions.size() + indexKey.positionIndex:
						indexKey.positionIndex -1;
					newMeshVertexPositions.push_back(uniqueVertexPositions[effectivePositionIndex]);
					if(doesMeshHaveNormals)
					{
						//This if either happens for all vertices, or for none, which is REALLY good.
						int effectivNormalIndex = indexKey.normalIndex < 0 ? uniqueVertexNormals.size() + indexKey.normalIndex:
						indexKey.normalIndex -1;
						newMeshVertexNormals.push_back(uniqueVertexNormals[effectivNormalIndex]);
					}
					
					if(doesMeshHaveTexcoords)
					{
						int effectiveTextureCoordIndex = indexKey.textureIndex < 0 ? uniqueVertexTexCoords.size() + indexKey.textureIndex:
						indexKey.textureIndex -1;
						newMeshTextureCoords.push_back(uniqueVertexTexCoords[effectiveTextureCoordIndex ]);
					}
					else
					{
						newMeshTextureCoords.push_back(glm::vec2(-1,-1));
					}

					//Lets also insert the new vertex
					unsigned int newVertexIndex = newMeshVertexPositions.size() - 1;
					threeIndexComponentToVertexIndex[indexKey] = newVertexIndex;

					polygonVertexIndices.push_back(newVertexIndex);
					++newlyCreatedVertexCount;
				}
			}

			if(!doesMeshHaveNormals)
			{
				//If we dont have normals vectors, we have to create them now.
				// for FLAT SHADING we have to push polygonsize amountonto the normal storage.
				glm::vec3 posA = newMeshVertexPositions[polygonVertexIndices[0]];
				glm::vec3 posB = newMeshVertexPositions[polygonVertexIndices[1]];
				glm::vec3 posC = newMeshVertexPositions[polygonVertexIndices[2]];

				glm::vec3 normal = glm::normalize(glm::cross(posB-posA,posC-posA));

				for(int i=0;i<newlyCreatedVertexCount;++i)
				{
					newMeshVertexNormals.push_back(normal);
				}
			}

			//Lets make the index triangles now.
			for (int i = 0;i < polygonSize - 2;++i)
			{
				glm::vec<3,int> newTriangleIndices;

				newTriangleIndices.x = polygonVertexIndices[0];
				newTriangleIndices.y = polygonVertexIndices[1 + i];
				newTriangleIndices.z = polygonVertexIndices[2 + i];

				newMeshTriangleIndices.push_back(newTriangleIndices);
			}
        }
    }
	

    return true;
}


AABB4 CalculateAABB4BasedOnTriangles(int startIndex, int endIndex,
	const std::vector<glm::vec3>& positions, 
	const std::vector<glm::vec<3,int>>& triangleIndices)
{
	glm::vec4 boxMax = { -FLT_MAX,-FLT_MAX,-FLT_MAX,0.0f };
	glm::vec4 boxMin = { FLT_MAX ,FLT_MAX ,FLT_MAX ,0.0f };

	for (int i = startIndex;i < endIndex;++i)
	{
		glm::ivec3 indices = triangleIndices[i];
		for(int j = 0;j < 3;++j)
		{
			glm::vec3 currentTriagVertPos = positions[(&indices.x)[j]]; //Ungabunga pointer

			if (currentTriagVertPos.x < boxMin.x)
			{
				boxMin.x = currentTriagVertPos.x;
			}
			if (currentTriagVertPos.y < boxMin.y)
			{
				boxMin.y = currentTriagVertPos.y;
			}
			if (currentTriagVertPos.z < boxMin.z)
			{
				boxMin.z = currentTriagVertPos.z;
			}

			if (currentTriagVertPos.x > boxMax.x)
			{
				boxMax.x = currentTriagVertPos.x;
			}
			if (currentTriagVertPos.y > boxMax.y)
			{
				boxMax.y = currentTriagVertPos.y;
			}
			if (currentTriagVertPos.z > boxMax.z)
			{
				boxMax.z = currentTriagVertPos.z;
			}
		}
	}

	
	return {boxMin,boxMax};
}

AABB3 GetWorldBoundsOfTransformedAABB(const glm::mat4 &worldTransform, const AABB3 &localAABB)
{
	AABB3 retval;
	retval.min = glm::vec3(FLT_MAX,FLT_MAX,FLT_MAX);
	retval.max = glm::vec3(-FLT_MAX,-FLT_MAX,-FLT_MAX);

    glm::vec3 sizeofLocalAABB = localAABB.max - localAABB.min;
	for(unsigned int i=0;i<8;++i)
	{
		glm::vec3 binaryWeights = glm::vec3(
				float((i >> 0) & 1u),
				float((i >> 1) & 1u),
				float((i >> 2) & 1u)
			);
		glm::vec3 cornerPoint = localAABB.min + sizeofLocalAABB * binaryWeights;

		glm::vec3 transformedCornerPoint = glm::vec3(worldTransform* glm::vec4(cornerPoint,1));
		//now we just feed this point to our aabb
		FeedVec3ToAABB3(retval, transformedCornerPoint);
	}

	return retval;
}

void FeedVec3ToAABB3(AABB3& eater, const glm::vec3& food)
{
	eater.min.x = fmin(eater.min.x,food.x);
	eater.min.y = fmin(eater.min.y,food.y);
	eater.min.z = fmin(eater.min.z,food.z);

	eater.max.x = fmax(eater.max.x,food.x);
	eater.max.y = fmax(eater.max.y,food.y);
	eater.max.z = fmax(eater.max.z,food.z);
}

void FeedAABB3ToAABB3(AABB3 &eater, const AABB3 &food)
{
	eater.min.x = fmin(eater.min.x,food.min.x);
	eater.min.y = fmin(eater.min.y,food.min.y);
	eater.min.z = fmin(eater.min.z,food.min.z);

	eater.max.x = fmax(eater.max.x,food.max.x);
	eater.max.y = fmax(eater.max.y,food.max.y);
	eater.max.z = fmax(eater.max.z,food.max.z);
}
