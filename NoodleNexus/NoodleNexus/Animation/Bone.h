#pragma once

#include <vector>
#include <assimp/scene.h>
#include <list>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
//#include <glm/gtx/quaternion.hpp>
#include "GLMHelpers.h"

struct KeyPosition
{
	glm::vec3 position;
	float timestamp;
};

struct KeyRotation
{
	glm::quat orientation;
	float timestamp;
};

struct KeyScale
{
	glm::vec3 scale;
	float timestamp;
};

class Bone
{
	public:
		Bone(const std::string& name, int id, const aiNodeAnim* channel);

		void Update(float animationTime);

		glm::mat4 GetLocalTransform();
		std::string GetName() const;
		int GetId();
		int GetPositionIndex(float animationTime);
		int GetRotationIndex(float animationTime);
		int GetScaleIndex(float animationTime);

	private:
		std::vector<KeyPosition> keyPositions;
		std::vector<KeyRotation> keyRotations;
		std::vector<KeyScale> keyScales;

		int numPositions;
		int numRotations;
		int numScales;

		glm::mat4 localTransform;
		std::string name;
		int id;

		float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);

		glm::mat4 InterpolatePosition(float animationTime);
		glm::mat4 InterpolateRotation(float animationTime);
		glm::mat4 InterpolateScale(float animationTime);

};
