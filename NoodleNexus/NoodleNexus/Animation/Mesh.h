#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"

#include <string>
#include <vector>

#define MAX_BONE_INFLUENCE 4

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;

	int boneIds[MAX_BONE_INFLUENCE];

	float weights[MAX_BONE_INFLUENCE];
};

struct Texture
{
	unsigned int id;
	std::string type;
	std::string path;
};

class Mesh
{

public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indicies;
	std::vector<Texture> textures;

	unsigned int VAO;

	Mesh(std::vector<Vertex> inVertices, std::vector<unsigned int> inIndicies, std::vector<Texture> inTextures);
	

	/*void Draw(Shader& shader)
	{
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		unsigned int normalNr = 1;
		unsigned int heightNr = 1;

		for (unsigned int i = 0; i < textures.size(); ++i)
		{
			glActiveTexture(GL_TEXTURE0 + i);

			std::string number;
			std::string name = textures[i].type;

			if (name == "texture_diffuse")
				number = std::to_string(diffuseNr++);
			else if (name == "texture_specular")
				number = std::to_string(specularNr++);
			else if (name == "texture_normal")
				number = std::to_string(normalNr++);
			else if (name == "texture_height")
				number = std::to_string(heightNr++);

			glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);

			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indicies.size()), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glActiveTexture(GL_TEXTURE0);
	}*/
	void Draw(GLuint& program);

	void DrawDepth(GLuint& program);

private:
	unsigned int VBO, EBO;

	void SetupMesh();
	
};