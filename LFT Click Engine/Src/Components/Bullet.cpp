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
	if (timer >= animationTime)
	{
		timer = 0;
		speed *= -1;
	}
	trans->Move(0, speed * FrameRateController::getInstance().DeltaTime());
	
	timer += FrameRateController::getInstance().DeltaTime();
}

void Bullet::Deserialize(nlohmann::json j, GameObject* parent)
{
	this->parent = parent;
	animationTime = j["animationTime"];
	speed = j["speed"];
}

Component* Bullet::Clone(GameObject* newParent)
{
	Bullet* toReturn = new Bullet();
	toReturn->parent = newParent;
	toReturn->animationTime = animationTime;
	toReturn->speed = speed;
	toReturn->timer = 0;
	return toReturn;
}

//Bullet::Bullet(json j, GameObject* parent) : parent(parent)
//{
//	animationTime = j["animationTime"];
//	speed = j["speed"];
//}
