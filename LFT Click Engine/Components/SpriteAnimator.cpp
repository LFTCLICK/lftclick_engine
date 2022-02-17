#include "SpriteAnimator.h"
#include "../FrameRateController.h"

void SpriteAnimator::Start()
{
	draw = parent->getComponent<Drawable>();
	timer = 0;
	offset = 1.0f / spriteSheetWidth;
}

void SpriteAnimator::Update()
{
	if (timer >= frameTime)
	{
		timer = 0;
		draw->xOffset += offset;
		if (draw->xOffset >= .9)
			draw->xOffset = 0;
	}
	timer += FrameRateController::getInstance().DeltaTime();
}

Component* SpriteAnimator::Clone(GameObject* newParent)
{
	SpriteAnimator* toReturn = new SpriteAnimator();
	toReturn->parent = newParent;
	toReturn->frameTime = frameTime;
	toReturn->spriteSheetWidth = spriteSheetWidth;
	toReturn->spriteSheetHeight = spriteSheetHeight;
	return toReturn;
}

SpriteAnimator::SpriteAnimator(json j, GameObject* parent) : parent(parent)
{

	spriteSheetWidth = j["spriteSheetWidth"];
	spriteSheetHeight = j["spriteSheetHeight"];
	frameTime = j["frameTime"];
}
