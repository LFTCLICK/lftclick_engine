// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	AnimationHelper.cpp
// Author			:	Vance Howald
// Creation Date	:	2021/12/10
// Purpose			:	Animates grabables
// History			: 
// ---------------------------------------------------------------------------
#include "AnimationHelper.h"
#include "FrameRateController.h"

void AnimationHelper::Start()
{
	draw = parent->getComponent<Drawable>();
	timer = 0;
}

void AnimationHelper::Update()
{
	if (timer >= animationTime)
	{
		timer = 0;
		draw->xOffset += 1 / offsetDenominator;
		if (draw->xOffset >= 1.001)
			draw->xOffset = 0;
	}
	timer += FrameRateControler::getInstance().DeltaTime();
}

Component * AnimationHelper::Clone(GameObject * newParent)
{
	AnimationHelper* toReturn = new AnimationHelper();
	toReturn->parent = newParent;
	toReturn->animationTime = animationTime;
	toReturn->offsetDenominator = offsetDenominator;
	return toReturn;
}

AnimationHelper::AnimationHelper(json j, GameObject * parent): parent(parent)
{
	animationTime = j["animationTime"];
	offsetDenominator = j["offsetDenominator"];
}
