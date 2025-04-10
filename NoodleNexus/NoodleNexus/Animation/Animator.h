#pragma once

#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Animation.h"
#include "Bone.h"

class Animator
{
	public:
		Animator(Animation* animation);

		void UpdateAnimation(float dt);

		void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);
		std::vector<glm::mat4> GetFinalBoneMatrices();
		void SetCurrentAnimation(Animation* currentAnimation);
		bool IsAnimationCompleted(Animation* animation);
		float GetDeltaTime();
		float GetCurrentTime();
		void ResetCurrentTime();
		void SpeedUpAnimation(float value);
		float GetCurrentAnimationSpeed();
		Animation* GetCurrentAnimation();

	private:
		std::vector<glm::mat4> finalBoneMatrices;
		Animation* currentAnimation;
		float currentTime;
		float deltaTime;
		float animationSpeed = 1.0f;
		bool bAmplifySpeed = false;
};