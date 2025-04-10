#pragma once

#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <functional>

#include "GLMHelpers.h"
#include "Bone.h"
#include "Model.h"
//#include "../sMesh.h"
//#include "../cVAOManager/cVAOManager.h"

struct AssimpNodeData
{
	glm::mat4 transformation;
	std::string name;
	int childrenCount;
	std::vector<AssimpNodeData> children;
};

class Animation
{
public:
	Animation() = default;
	Animation(const std::string& animationPath, Model* model);
	~Animation();

	Bone* FindBone(const std::string& name);

	float GetTicksPerSecond();
	float GetDuration();
	const AssimpNodeData& GetRootNode();
	const std::map<std::string, BoneInfo>& getBoneInfoMap();

	bool GetLooping(Animation* animation);
	void SetLooping(Animation* animation, bool isLooping);

	private:
		bool bIsLooping = true;
		float duration;
		int ticksPerSecond;
		std::vector<Bone> bones;
		AssimpNodeData rootNode;
		std::map<std::string, BoneInfo> boneInfoMap;

		void ReadMissingBones(const aiAnimation* animation, Model& model);
		void ReadHierarchyData(AssimpNodeData& dest, const aiNode* src);
};
