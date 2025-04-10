#include "Animation.h"

Animation::Animation(const std::string& animationPath, Model* model)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);


	assert(scene && scene->mRootNode);
	aiAnimation* animation = scene->mAnimations[0];
	duration = animation->mDuration;
	ticksPerSecond = animation->mTicksPerSecond;
	aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
	globalTransformation = globalTransformation.Inverse();
	ReadHierarchyData(rootNode, scene->mRootNode);
	ReadMissingBones(animation, *model);
}

Animation::~Animation()
{
}

Bone* Animation::FindBone(const std::string& name)
{
	std::vector<Bone>::iterator iter = std::find_if(bones.begin(), bones.end(),
		[&](const Bone& bone)
		{
			return bone.GetName() == name;
		}
	);
	if (iter == bones.end()) return nullptr;
	else return &(*iter);
}

float Animation::GetTicksPerSecond()
{
	return ticksPerSecond;
}

float Animation::GetDuration()
{
	return duration;
}

const AssimpNodeData& Animation::GetRootNode()
{
	return rootNode;
}

const std::map<std::string, BoneInfo>& Animation::getBoneInfoMap()
{
	return boneInfoMap;
}

bool Animation::GetLooping(Animation* animation)
{
	return animation->bIsLooping;
}

void Animation::SetLooping(Animation* animation, bool isLooping)
{
	animation->bIsLooping = isLooping;
}

void Animation::ReadMissingBones(const aiAnimation* animation, Model& model)
{
	int size = animation->mNumChannels;

	std::map<std::string, BoneInfo>& bInfoMap = model.GetBoneInfoMap();
	int& bCount = model.GetBoneCounter();

	for (int i = 0; i < size; ++i)
	{
		aiNodeAnim* channel = animation->mChannels[i];
		std::string boneName = channel->mNodeName.data;

		if (bInfoMap.find(boneName) == bInfoMap.end())
		{
			bInfoMap[boneName].id = bCount;
			bCount++;
		}
		bones.push_back(Bone(channel->mNodeName.data,
			bInfoMap[channel->mNodeName.data].id, channel));
	}

	boneInfoMap = bInfoMap;

}

void Animation::ReadHierarchyData(AssimpNodeData& dest, const aiNode* src)
{
	assert(src);

	dest.name = src->mName.data;
	dest.transformation = GLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
	dest.childrenCount = src->mNumChildren;

	for (int i = 0; i < src->mNumChildren; ++i)
	{
		AssimpNodeData newData;
		ReadHierarchyData(newData, src->mChildren[i]);
		dest.children.push_back(newData);
	}
}