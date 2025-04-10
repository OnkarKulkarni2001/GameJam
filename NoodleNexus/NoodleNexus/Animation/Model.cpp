#include "Model.h"
#include <glm/gtc/matrix_transform.hpp>
#include "GLMHelpers.h"
#include "Animator.h"

extern Animator* g_pAnimator;

Model::Model(std::string const& path, bool gamma)
{
	gammacorrection = gamma;
	LoadModel(path);
}

Model::Model(const Model& other) : gammacorrection(other.gammacorrection), directory(other.directory), boneCounter(other.boneCounter)
{
	textures_loaded = other.textures_loaded;
	meshes.reserve(other.meshes.size());
	for (const Mesh& mesh : other.meshes) {
		meshes.push_back(Mesh(mesh));
	}
	boneInfoMap = other.boneInfoMap;
}

Model& Model::operator=(const Model& other)
{
	if (this != &other) {
		meshes.clear();
		textures_loaded.clear();
		boneInfoMap.clear();

		gammacorrection = other.gammacorrection;
		directory = other.directory;
		boneCounter = other.boneCounter;

		textures_loaded = other.textures_loaded;
		meshes.reserve(other.meshes.size());
		for (const Mesh& mesh : other.meshes) {
			meshes.push_back(Mesh(mesh));
		}
		boneInfoMap = other.boneInfoMap;
	}
	return *this;
}

void Model::Draw(GLuint& program)
{
	for (unsigned int i = 0; i < meshes.size(); ++i)
	{
		meshes[i].Draw(program);
	}
}

void Model::DrawDepth(GLuint& program)
{
	for (unsigned int i = 0; i < meshes.size(); ++i)
	{
		meshes[i].DrawDepth(program);
	}
}

void Model::RenderAnimatedStuff(GLuint& program, glm::mat4& matProjection, glm::mat4& matView, double deltaTime)
{
	//--------------------------------------------------------------------------------Animations----------------------------------------------------------------------
	g_pAnimator->UpdateAnimation(deltaTime);

	GLint matProjection_UL = glGetUniformLocation(program, "matProjection");
	GLint matView_UL = glGetUniformLocation(program, "matView");
	glUniformMatrix4fv(matProjection_UL, 1, GL_FALSE, (const GLfloat*)&matProjection);

	glUniformMatrix4fv(matView_UL, 1, GL_FALSE, (const GLfloat*)&matView);


	std::vector<glm::mat4> transforms = g_pAnimator->GetFinalBoneMatrices();
	for (int i = 0; i < transforms.size(); ++i)
	{
		std::string name = "finalBonesMatrices[" + std::to_string(i) + "]";
		GLint transform_UL = glGetUniformLocation(program, name.c_str());
		glUniformMatrix4fv(transform_UL, 1, GL_FALSE, (const GLfloat*)&transforms[i]);
	}

	glm::mat4 matModel = glm::mat4(1.0f);

	glm::mat4 matTranslate
		= glm::translate(glm::mat4(1.0f),
			glm::vec3(positionXYZ.x,
				positionXYZ.y,
				positionXYZ.z));

	// Rotation...
	// Caculate 3 Euler acix matrices...
	glm::mat4 matRotateX =
		glm::rotate(glm::mat4(1.0f),
			glm::radians(rotationEulerXYZ.x), // Angle in radians
			glm::vec3(1.0f, 0.0, 0.0f));

	glm::mat4 matRotateY =
		glm::rotate(glm::mat4(1.0f),
			glm::radians(rotationEulerXYZ.y), // Angle in radians
			glm::vec3(0.0f, 1.0, 0.0f));

	glm::mat4 matRotateZ =
		glm::rotate(glm::mat4(1.0f),
			glm::radians(rotationEulerXYZ.z), // Angle in radians
			glm::vec3(0.0f, 0.0, 1.0f));


	// Scale
	glm::mat4 matScale = glm::scale(glm::mat4(1.0f),
		glm::vec3(uniformScale,
			uniformScale,
			uniformScale));


	matModel *= matTranslate;     // matModel = matModel * matTranslate;
	matModel *= matRotateX;
	matModel *= matRotateY;
	matModel *= matRotateZ;
	matModel *= matScale;


	GLint matModel_UL = glGetUniformLocation(program, "matModel");
	glUniformMatrix4fv(matModel_UL, 1, GL_FALSE, (const GLfloat*)&matModel);

	Draw(program);
	//--------------------------------------------------------------------------------Animations----------------------------------------------------------------------
}

std::map<std::string, BoneInfo>& Model::GetBoneInfoMap()
{
	return boneInfoMap;
}

int& Model::GetBoneCounter()
{
	return boneCounter;
}

void Model::LoadModel(std::string const& path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR: ASSIMP: " << importer.GetErrorString() << std::endl;
		return;
	}

	directory = path.substr(0, path.find_last_of("/"));

	ProcessNode(scene->mRootNode, scene);
}

void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(ProcessMesh(mesh, scene));
	}

	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}

Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indicies;
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	{
		Vertex vertex;
		SetVertexBoneDataToDefault(vertex);
		vertex.Position = GLMHelpers::GetGLMVec(mesh->mVertices[i]);
		vertex.Normal = GLMHelpers::GetGLMVec(mesh->mNormals[i]);

		if (mesh->mTextureCoords[0])
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
		{
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		}

		vertices.push_back(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; ++j)
		{
			indicies.push_back(face.mIndices[j]);
		}
	}

	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	std::vector<Texture> diffuseMaps = LoadMaterialTexture(material, aiTextureType_DIFFUSE, "texture_diffuse");
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	std::vector<Texture> specularMaps = LoadMaterialTexture(material, aiTextureType_SPECULAR, "texture_specular");
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	std::vector<Texture> normalMaps = LoadMaterialTexture(material, aiTextureType_NORMALS, "texture_normal");
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	std::vector<Texture> heightMaps = LoadMaterialTexture(material, aiTextureType_HEIGHT, "texture_height");
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

	ExtractBoneWeightForVerticies(vertices, mesh, scene);

	return Mesh(vertices, indicies, textures);
}

void Model::SetVertexBoneDataToDefault(Vertex& vertex)
{
	for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
	{
		vertex.boneIds[i] = -1;
		vertex.weights[i] = 0;
	}
}

void Model::SetVertexBoneData(Vertex& vertex, int boneId, float weight)
{
	for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
	{
		if (vertex.boneIds[i] < 0)
		{
			vertex.weights[i] = weight;
			vertex.boneIds[i] = boneId;
			break;
		}
	}
}

void Model::ExtractBoneWeightForVerticies(std::vector<Vertex>& verticies, aiMesh* mesh, const aiScene* scene)
{
	std::map<std::string, BoneInfo>& bInfoMap = boneInfoMap;
	int& bCount = boneCounter;

	for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
	{
		int boneId = -1;
		std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
		if (bInfoMap.find(boneName) == bInfoMap.end())
		{
			BoneInfo newBoneInfo;
			newBoneInfo.id = bCount;
			newBoneInfo.offset = GLMHelpers::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
			bInfoMap[boneName] = newBoneInfo;
			boneId = bCount;
			bCount++;
		}
		else
		{
			boneId = bInfoMap[boneName].id;
		}
		aiVertexWeight* weights = mesh->mBones[boneIndex]->mWeights;
		int numWeights = mesh->mBones[boneIndex]->mNumWeights;

		for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
		{
			int vertexId = weights[weightIndex].mVertexId;
			float weight = weights[weightIndex].mWeight;
			assert(vertexId <= verticies.size());
			SetVertexBoneData(verticies[vertexId], boneId, weight);
		}
	}
}

unsigned int Model::TextureFromFile(const char* path, const std::string& directory, bool gamma)
{
	std::string filename = std::string(path);
	filename = directory + '/' + filename;  // Full file path

	// Create an instance of CTextureFromBMP (or use an existing one)
	CTextureFromBMP textureLoader;

	// Use the texture file name (without path) as the texture name
	std::string textureName = std::string(path);  // Could extract just the filename if needed

	// Call CreateNewTextureFromBMPFile2
	bool success = textureLoader.CreateNewTextureFromBMPFile2(
		textureName,        // Texture name (e.g., "texture1.bmp")
		filename,           // Full file path
		true                // Generate MIP maps (matches original behavior)
	);

	if (success)
	{
		return textureLoader.getTextureNumber();  // Return the generated texture ID
	}
	else
	{
		std::cout << "Texture failed to load at path: " << filename << std::endl;
		return 0;  // Return 0 to indicate failure (common OpenGL convention)
	}
}

std::vector<Texture> Model::LoadMaterialTexture(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		bool skip = false;

		for (unsigned int j = 0; j < textures_loaded.size(); ++j)
		{
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}

		if (!skip)
		{
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), this->directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture);
		}

		return textures;
	}
}