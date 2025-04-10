#pragma once

// This is the 3D drawing information
//	to draw a single mesh (single PLY file)

#include <string>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <vector>
#include <string>

//struct Texture
//{
//	unsigned int id;
//	std::string type;
//	std::string path;
//};

struct sMesh
{
	sMesh();
	std::string modelFileName;			// "bunny.ply"
	std::string uniqueFriendlyName;		// "Fred", "Ali", "Bunny_007"

	glm::vec3 positionXYZ;
	glm::vec3 rotationEulerXYZ;		// 90 degrees around the x axis
	float uniformScale = 1.0f;				// Same for each axis

	// These two matrices are going to be used before 
	//	and after the transformations above
	//
	// "pre" moves the object to the local origin we want
	glm::mat4 matPreParentRelative = glm::mat4(1.0f);
	// "post" moves it back to where it's supposed to be 
	//	relative to the parent
	glm::mat4 matPostParentRelative = glm::mat4(1.0f);

	glm::mat4 calcMatModel(void);

	glm::vec4 objectColourRGBA;		// 0 - 1.0 
	// If true, it uses the colour above
	bool bOverrideObjectColour = true;

	bool bIsWireframe = false;
	bool bIsVisible = true;
	bool bDoNotLight = false;

	// unique ID is read only
	unsigned int getUniqueID(void);
private:
	unsigned int m_uniqueID = 0;
	// 
	static unsigned int m_NextUniqueID;// = 100;
public:

//	std::string textureName;
	static const unsigned int MAX_NUM_TEXTURES = 8;
	std::string textures[MAX_NUM_TEXTURES];
	float blendRatio[MAX_NUM_TEXTURES];

	// 0.0 = invisible
	// 1.0 = solid 
	float alphaTransparency = 1.0f;

	std::string getState(void);
	bool loadState(std::string newState);

//	sMesh* pChildMeshes[100];
	std::vector< sMesh* > vec_pChildMeshes;

	sMesh* pFindChildMeshByFriendlyName(std::string theNameToFind);

};