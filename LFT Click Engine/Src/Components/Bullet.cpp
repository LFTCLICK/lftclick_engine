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
#include "Collider.h"
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

	if (!liveForever) {
		timer += FrameRateController::getInstance().DeltaTime();
		if (timer > keepAliveTime) parent->isDeletable = true;
	}
}

void Bullet::Deserialize(nlohmann::json j, GameObject* parent)
{
	this->parent = parent;
	animationTime = j["animationTime"];
	keepAliveTime = j["keepAliveTime"];
	speed = j["speed"];
	liveForever = false;
}

void Bullet::HandleMessage(Message* e)
{
	if(!e->otherObject->isTrigger && e->otherObject->parent->tag!="door")
		parent->isDeletable = true;
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
