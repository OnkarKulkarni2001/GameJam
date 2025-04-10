#pragma once




#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <map>
#include <string>

class cPhysicsObjectTypes
{
public:
	enum eShapeType
	{
		BOX,		// Params: "half-extents": x,y,z 

		SPHERE,		// Params: "radius": x

		CAPSULE,
		MESH,
		// ...etc.

		UNKNOWN
	};

	eShapeType shapeType = UNKNOWN;

	// Orientation of the object
	glm::mat4 matModel = glm::mat4(1.0f);

	// Storing the various types of attributes for the various geometry 
	//	by saving them in a set of name-value pairs
	std::map< std::string /*param name*/, glm::vec4 /*value*/ > map_ParamToValue;


	// You could also store the various different things for each shape 
	//  this way, where you set the ones that are appropriate and ignore the 
	//	ones that don't apply

//	// Is box
//	glm::vec3 minXYZ, maxXYZ, centreXYZ;
//
//	// Is sphere
//	glm::vec3 centreXYZ;
//	float radius;
//
//	// Is capsule
//	// https://docs.nvidia.com/gameworks/content/gameworkslibrary/physx/guide/Manual/Geometry.html
//	glm::vec3 originXYZ;
//	float halfHeight;
//	float radius;
//
//	// And so on, for all types you want to draw


};
