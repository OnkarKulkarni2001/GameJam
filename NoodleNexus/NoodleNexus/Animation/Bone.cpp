#include "Bone.h"

Bone::Bone(const std::string& name, int id, const aiNodeAnim* channel)
{
	this->name = name;
	this->id = id;
	localTransform = 1.0f;

	numPositions = channel->mNumPositionKeys;

	for (int positionIndex = 0; positionIndex < numPositions; ++positionIndex)
	{
		aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
		float timeStamp = channel->mPositionKeys[positionIndex].mTime;
		KeyPosition data;
		data.position = GLMHelpers::GetGLMVec(aiPosition);
		data.timestamp = timeStamp;
		keyPositions.push_back(data);
	}

	numRotations = channel->mNumRotationKeys;

	for (int rotationIndex = 0; rotationIndex < numRotations; ++rotationIndex)
	{
		aiQuaternion aiRotation = channel->mRotationKeys[rotationIndex].mValue;
		float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
		KeyRotation data;
		data.orientation = GLMHelpers::GetGLMQuat(aiRotation);
		data.timestamp = timeStamp;
		keyRotations.push_back(data);
	}

	numScales = channel->mNumScalingKeys;

	for (int scaleIndex = 0; scaleIndex < numScales; ++scaleIndex)
	{
		aiVector3D aiScale = channel->mScalingKeys[scaleIndex].mValue;
		float timeStamp = channel->mPositionKeys[scaleIndex].mTime;
		KeyScale data;
		data.scale = GLMHelpers::GetGLMVec(aiScale);
		data.timestamp = timeStamp;
		keyScales.push_back(data);
	}
}

void Bone::Update(float animationTime)
{
	glm::mat4 translation = InterpolatePosition(animationTime);
	glm::mat4 rotation = InterpolateRotation(animationTime);
	glm::mat4 scale = InterpolateScale(animationTime);
	localTransform = translation * rotation * scale;
}

glm::mat4 Bone::GetLocalTransform()
{
	return localTransform;
}

std::string Bone::GetName() const
{
	return name;
}

int Bone::GetId()
{
	return id;
}

int Bone::GetPositionIndex(float animationTime)
{
	for (int index = 0; index < numPositions - 1; ++index)
	{
		if (animationTime < keyPositions[index + 1].timestamp)
			return index;
	}
}

int Bone::GetRotationIndex(float animationTime)
{
	for (int index = 0; index < numRotations - 1; ++index)
	{
		if (animationTime < keyRotations[index + 1].timestamp)
			return index;
	}
}

int Bone::GetScaleIndex(float animationTime)
{
	for (int index = 0; index < numScales - 1; ++index)
	{
		if (animationTime < keyScales[index + 1].timestamp)
			return index;
	}
}

float Bone::GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
{
	float scaleFactor = 0.0f;
	float midwayLength = animationTime - lastTimeStamp;
	float frameeDiff = nextTimeStamp - lastTimeStamp;
	scaleFactor = midwayLength / frameeDiff;
	return scaleFactor;
}

glm::mat4 Bone::InterpolatePosition(float animationTime)
{
	if (numPositions == 1)
		return glm::translate(glm::mat4(1.0f), keyPositions[0].position);

	int p0Index = GetPositionIndex(animationTime);
	int p1Index = p0Index + 1;
	float scaleFactor = GetScaleFactor(keyPositions[p0Index].timestamp, keyPositions[p1Index].timestamp, animationTime);
	glm::vec3 finalPosition = glm::mix(keyPositions[p0Index].position, keyPositions[p1Index].position, scaleFactor);
	return glm::translate(glm::mat4(1.0f), finalPosition);
}

glm::mat4 Bone::InterpolateRotation(float animationTime)
{
	if (numRotations == 1)
	{
		glm::quat rotation = glm::normalize(keyRotations[0].orientation);/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		return glm::mat4_cast(rotation);
	}

	int p0Index = GetRotationIndex(animationTime);
	int p1Index = p0Index + 1;
	float scaleFactor = GetScaleFactor(keyRotations[p0Index].timestamp,
		keyRotations[p1Index].timestamp, animationTime);
	glm::quat finalRotation = glm::slerp(keyRotations[p0Index].orientation,
		keyRotations[p1Index].orientation, scaleFactor);
	finalRotation = glm::normalize(finalRotation);
	return glm::mat4_cast(finalRotation);
}

glm::mat4 Bone::InterpolateScale(float animationTime)
{
	if (numScales == 1)
		return glm::translate(glm::mat4(1.0f), keyScales[0].scale);

	int p0Index = GetScaleIndex(animationTime);
	int p1Index = p0Index + 1;
	float scaleFactor = GetScaleFactor(keyScales[p0Index].timestamp,
		keyScales[p1Index].timestamp, animationTime);
	glm::vec3 finalScale = glm::mix(keyScales[p0Index].scale,
		keyScales[p1Index].scale, scaleFactor);
	return glm::scale(glm::mat4(1.0f), finalScale);
}