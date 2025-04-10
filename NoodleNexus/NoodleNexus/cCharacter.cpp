#include "cCharacter.h"
extern Animator* g_pAnimator;

cCharacter::cCharacter(Model& model)
{
	this->characterModel = &model;
}

cCharacter::~cCharacter()
{
}

//void cCharacter::SetAnimation(Animation* animation)
//{
//	currentAnimation = *animation;
//}

void cCharacter::AddAnimation(Animation* animation)
{
	vecAnimations.push_back(animation);
}

void cCharacter::PlayAnimation(Animation* animation, float animationSpeed)
{
	if (g_pAnimator->GetCurrentAnimation() != animation) {
		currentAnimation = animation;
		g_pAnimator->SetCurrentAnimation(animation);
		g_pAnimator->ResetCurrentTime();
		g_pAnimator->SpeedUpAnimation(animationSpeed);
	}
}

