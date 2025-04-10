#include "Mesh.h"
extern GLuint program;

Mesh::Mesh(std::vector<Vertex> inVertices, std::vector<unsigned int> inIndicies, std::vector<Texture> inTextures)
{
	vertices = inVertices;
	indicies = inIndicies;
	textures = inTextures;

	SetupMesh();
}

void Mesh::Draw(GLuint& program)
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

		glUniform1i(glGetUniformLocation(program, (name + number).c_str()), i);

		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indicies.size()), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
}

void Mesh::DrawDepth(GLuint& program)
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

		glUniform1i(glGetUniformLocation(program, (name + number).c_str()), i);

		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}

	glBindVertexArray(VAO);
	//glDrawElementsInstanced(GL_TRIANGLES,
	//	static_cast<unsigned int>(indicies.size()),
	//	GL_UNSIGNED_INT,
	//	(void*)0,
	//	6);  // Render 6 instances (one per cubemap face)
	glDrawElements(GL_TRIANGLES,
		static_cast<unsigned int>(indicies.size()),
		GL_UNSIGNED_INT,
		(void*)0);
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
}

void Mesh::SetupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies.size() * sizeof(unsigned int), &indicies[0], GL_STATIC_DRAW);

	GLint vpos_location = glGetAttribLocation(program, "vPos");			// in vec3 vPos;
	//GLint vcol_location = glGetAttribLocation(program, "vCol");			// in vec3 vCol;
	GLint vnorm_location = glGetAttribLocation(program, "vNormal");		// in vec3 vNormal;
	GLint vUV_location = glGetAttribLocation(program, "vUV");			// in vec2 vUV;

	GLint tangent_location = glGetAttribLocation(program, "tangent");			//in vec3 tangent;
	GLint bitangent_location = glGetAttribLocation(program, "bitangent");		//in vec3 bitangent;
	GLint boneIds_location = glGetAttribLocation(program, "boneIds");			//in ivec4 boneIds;
	GLint weights_location = glGetAttribLocation(program, "weights");			//in vec4 weights;

	glEnableVertexAttribArray(vpos_location);
	glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	glEnableVertexAttribArray(vnorm_location);
	glVertexAttribPointer(vnorm_location, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

	glEnableVertexAttribArray(vUV_location);
	glVertexAttribPointer(vUV_location, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

	glEnableVertexAttribArray(tangent_location);
	glVertexAttribPointer(tangent_location, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

	glEnableVertexAttribArray(bitangent_location);
	glVertexAttribPointer(bitangent_location, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

	glEnableVertexAttribArray(boneIds_location);
	glVertexAttribIPointer(boneIds_location, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, boneIds));

	glEnableVertexAttribArray(weights_location);
	glVertexAttribPointer(weights_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, weights));


	/*glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

	glEnableVertexAttribArray(5);
	glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, boneIds));

	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, weights));*/

	glBindVertexArray(0);
}