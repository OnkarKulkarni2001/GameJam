#pragma once

#include "../Animation/Animator.h"
#include "../Animation/Animation.h"
#include "../Animation/Model.h"


class cCharacter
{
public:
	cCharacter(Model& model);
	~cCharacter();

	//void SetAnimation(Animation* animation);
	void AddAnimation(Animation* animation);
	void PlayAnimation(Animation* animation, float animationSpeed = 1.0f);

	Model* characterModel;
	std::vector<Animation*> vecAnimations;
	Animation* currentAnimation;
};