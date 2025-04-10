#include "Animator.h"

Animator::Animator(Animation* animation)
{
	deltaTime = 0.0f;
	currentTime = 0.0f;
	currentAnimation = animation;

	finalBoneMatrices.reserve(100);

	for (int i = 0; i < 100; ++i)
	{
		finalBoneMatrices.push_back(glm::mat4(1.0f));
	}
}

void Animator::UpdateAnimation(float dt)
{
	deltaTime = dt ;

	if (currentAnimation && currentAnimation->GetLooping(currentAnimation))
	{
		currentTime += currentAnimation->GetTicksPerSecond() * dt * animationSpeed;
		currentTime = fmod(currentTime, currentAnimation->GetDuration());
		CalculateBoneTransform(&currentAnimation->GetRootNode(), glm::mat4(1.0f));
	}
	else if (currentAnimation)  // Check if animation exists for the else case
	{
		currentTime += currentAnimation->GetTicksPerSecond() * dt;
		// Clamp the time to not exceed duration for non-looping animations
		if (currentTime >= currentAnimation->GetDuration())
		{
			currentTime = currentAnimation->GetDuration();  // Stop at the end
		}
		CalculateBoneTransform(&currentAnimation->GetRootNode(), glm::mat4(1.0f));
	}
}

void Animator::CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
{
	std::string nodeName = node->name;
	glm::mat4 nodeTransform = node->transformation;

	Bone* bone = currentAnimation->FindBone(nodeName);

	if (bone)
	{
		if (currentAnimation->GetLooping(currentAnimation)) {
			bone->Update(currentTime);
			nodeTransform = bone->GetLocalTransform();
		}
		else {
			bone->Update(currentTime - deltaTime);
		}
		nodeTransform = bone->GetLocalTransform();
	}

	glm::mat4 globalTransformation = parentTransform * nodeTransform;

	std::map<std::string, BoneInfo> boneInfoMap = currentAnimation->getBoneInfoMap();

	if (boneInfoMap.find(nodeName) != boneInfoMap.end())
	{
		int index = boneInfoMap[nodeName].id;
		glm::mat4 offset = boneInfoMap[nodeName].offset;
		finalBoneMatrices[index] = globalTransformation * offset;
	}

	for (int i = 0; i < node->childrenCount; ++i)
	{
		CalculateBoneTransform(&node->children[i], globalTransformation);
	}
}

std::vector<glm::mat4> Animator::GetFinalBoneMatrices()
{
	return finalBoneMatrices;
}

void Animator::SetCurrentAnimation(Animation* currentAnimation)
{
	this->currentAnimation = currentAnimation;
}

bool Animator::IsAnimationCompleted(Animation* animation)
{
	if (!animation) return true;  // If no animation, consider it "completed"

	// If animation is looping, it's never truly completed
	if (animation->GetLooping(animation)) {
		return false;
	}

	// For non-looping animations, check if currentTime has reached or exceeded duration
	return currentTime >= animation->GetDuration();
}

float Animator::GetDeltaTime()
{
	return deltaTime;
}

float Animator::GetCurrentTime()
{
	return currentTime;
}

void Animator::ResetCurrentTime()
{
	this->currentTime = 0.0f;
}

void Animator::SpeedUpAnimation(float value)
{
	bAmplifySpeed = true;
	animationSpeed = value;
}

float Animator::GetCurrentAnimationSpeed()
{
	return animationSpeed;
}

Animation* Animator::GetCurrentAnimation()
{
	return currentAnimation;
}