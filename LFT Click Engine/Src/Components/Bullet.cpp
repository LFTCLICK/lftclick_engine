// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	Bullet.cpp
// Author			:	Vance Howald
// Creation Date	:	2021/12/10
// Purpose			:	Bullet header for player
// History			:
// ---------------------------------------------------------------------------
#include "pch.h"
#include "Bullet.h"
#include "Collider.h"
#include "FrameRateController.h"


void Bullet::Start()
{
	myTransform = componentOwner->getComponent<Transform>();
	timer = 0;
}

void Bullet::Update()
{
	myTransform->Move(
		direction.x * speed * g_FrameRateController->DeltaTime(),
		direction.y * speed * g_FrameRateController->DeltaTime()
	);

	if (!liveForever) {
		timer += g_FrameRateController->DeltaTime();
		if (timer > keepAliveTime) componentOwner->isDeletable = true;
	}
}

void Bullet::Deserialize(nlohmann::json j, GameObject* componentOwner)
{
	this->componentOwner = componentOwner;
	animationTime = j["animationTime"];
	keepAliveTime = j["keepAliveTime"];
	speed = j["speed"];
	liveForever = false;
}

void Bullet::HandleMessage(Message* e)
{
	if(!e->otherObject->isTrigger && e->otherObject->componentOwner->tag!="door")
		componentOwner->isDeletable = true;
}

Component* Bullet::Clone(GameObject* newParent)
{
	Bullet* toReturn = new Bullet();
	toReturn->componentOwner = newParent;
	toReturn->animationTime = animationTime;
	toReturn->speed = speed;
	toReturn->direction = direction;
	toReturn->timer = 0;
	toReturn->keepAliveTime = keepAliveTime;
	toReturn->myTransform = myTransform;
	return toReturn;
}
