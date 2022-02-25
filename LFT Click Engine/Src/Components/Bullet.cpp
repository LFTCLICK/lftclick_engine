// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	AnimationHelper.cpp
// Author			:	Vance Howald
// Creation Date	:	2021/12/10
// Purpose			:	Animates grabables
// History			:
// ---------------------------------------------------------------------------
#include "pch.h"
#include "Bullet.h"
#include "FrameRateController.h"


void Bullet::Start()
{
	trans = parent->getComponent<Transform>();
	timer = 0;
}

void Bullet::Update()
{
	trans->Move(
		direction.x * speed * FrameRateController::getInstance().DeltaTime(),
		direction.y * speed * FrameRateController::getInstance().DeltaTime()
	);
	/* Currently not working as I don't know how to let the GOM know about object deletion.

	if (!liveForever) {
		timer += FrameRateController::getInstance().DeltaTime();
		if (timer > keepAliveTime)
			delete parent;
	}
	*/


	/* Older Bullet code for reference
	if (timer >= animationTime)
	{
		timer = 0;
		speed *= -1;
	}
	trans->Move(0, speed * FrameRateController::getInstance().DeltaTime());

	timer += FrameRateController::getInstance().DeltaTime();
	*/
}

void Bullet::Deserialize(nlohmann::json j, GameObject* parent)
{
	this->parent = parent;
	animationTime = j["animationTime"];
	keepAliveTime = j["keepAliveTime"];
	speed = j["speed"];
	liveForever = false;
}

Component* Bullet::Clone(GameObject* newParent)
{
	Bullet* toReturn = new Bullet();
	toReturn->parent = newParent;
	toReturn->animationTime = animationTime;
	toReturn->speed = speed;
	toReturn->direction = direction;
	toReturn->timer = 0;
	toReturn->keepAliveTime = keepAliveTime;
	toReturn->trans = trans;
	return toReturn;
}
