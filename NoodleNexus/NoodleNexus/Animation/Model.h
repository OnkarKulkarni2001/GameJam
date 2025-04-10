#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>


#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

#include "Mesh.h"
#include "Shader.h"

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>

#include "../cBasicTextureManager/CTextureFromBMP.h"

struct BoneInfo
{
	int id;

	glm::mat4 offset;
};

class Model
{
public:
	std::vector<Texture> textures_loaded;
	std::vector<Mesh> meshes;
	std::string directory;
	bool gammacorrection;

	glm::vec3 positionXYZ;
	glm::vec3 rotationEulerXYZ;		// 90 degrees around the x axis
	float uniformScale = 1.0f;				// Same for each axis

	Model(std::string const& path, bool gamma = false);

	
	// Copy constructor
	Model(const Model& other);
	

	// Assignment operator
	Model& operator=(const Model& other);

	//void Draw(Shader& shader)
	//{
	//	for (unsigned int i = 0; i < meshes.size(); ++i)
	//	{
	//		meshes[i].Draw(shader);
	//	}
	//}
	void Draw(GLuint& program);

	void DrawDepth(GLuint& program);

	void RenderAnimatedStuff(GLuint& program, glm::mat4& matProjection, glm::mat4& matView, double deltaTime);

	std::map<std::string, BoneInfo>& GetBoneInfoMap();
	int& GetBoneCounter();

private:

	std::map<std::string, BoneInfo> boneInfoMap;
	int boneCounter = 0;

	void LoadModel(std::string const& path);
	

	void ProcessNode(aiNode* node, const aiScene* scene);



	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	

	void SetVertexBoneDataToDefault(Vertex& vertex);


	void SetVertexBoneData(Vertex& vertex, int boneId, float weight);


	void ExtractBoneWeightForVerticies(std::vector<Vertex>& verticies, aiMesh* mesh, const aiScene* scene);
	

	/*unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false)
	{
		std::string filename = std::string(path);
		filename = directory + '/' + filename;

		unsigned int textureId;
		glGenTextures(1, &textureId);

		int width, height, nrComponents;
		unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);

		if (data)
		{
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureId);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << path << std::endl;
			stbi_image_free(data);
		}

		return textureId;
	}*/
	unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);

	std::vector<Texture> LoadMaterialTexture(aiMaterial* mat, aiTextureType type, std::string typeName);

};
