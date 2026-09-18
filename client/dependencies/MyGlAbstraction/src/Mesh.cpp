#include "Mesh.h"

#include "Utils.h"


template<>
void Mesh<VertexP3N3T2,RenderType::Triangles>::Load(const std::string& filePath)
{
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texCoords;
	std::vector<glm::ivec3> triangleIndices;

	std::vector<unsigned int> indices;
	std::vector<VertexP3N3T2> vertices;

	bool wasLoadingSuccesful = TryLoadObjFile(filePath,positions,normals,texCoords,triangleIndices);
	
	if(wasLoadingSuccesful)
	{
		vertices.resize(positions.size());	
		for(int i=0;i<positions.size();++i)
		{
			vertices[i] = {positions[i],normals[i],texCoords[i]};
		}

		indices.resize(triangleIndices.size()*3);
		for(int i = 0;i<triangleIndices.size();++i)
		{
			indices[i*3] = triangleIndices[i].x;
			indices[i*3 + 1] = triangleIndices[i].y;
			indices[i*3 + 2] = triangleIndices[i].z;
		}

		Init(vertices, indices);
	}
}