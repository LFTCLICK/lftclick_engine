// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	Gun.cpp
// Author			:	Chris Fitch
// Creation Date	:	2022/02/24
// Purpose			:	Contains functions for firing bullets
// History			: 
// ---------------------------------------------------------------------------
#include "pch.h"
#include "Gun.h"
#include "Bullet.h"
#include "FrameRateController.h"
using json = nlohmann::json;

void Gun::Start()
{
	trans = parent->getComponent<Transform>();
	timer = 0;
}

void Gun::Update()
{
	timer += FrameRateController::getInstance().DeltaTime();
}

Component* Gun::Clone(GameObject* newParent)
{
	Gun* toReturn = new Gun();
	toReturn->parent = newParent;
	toReturn->timer = 0;
	toReturn->bulletTypes = bulletTypes;
	return toReturn;
}

void Gun::Fire(int bulletIndex, float targetX, float targetY)
{
	GameObject* bullet = gom->CloneObject(bulletTypes[bulletIndex].first);
	Transform* bulletTransform = bullet->getComponent<Transform>();
	Bullet* bulletComp = bullet->getComponent<Bullet>();
	DirectX::SimpleMath::Vector2 parentPos = trans->CurrentPos();

	DirectX::SimpleMath::Vector2 target(targetX, targetY);
	bulletComp->direction = target - parentPos;
	bulletComp->direction.Normalize();
	bulletTransform->SetPos(parentPos.x + bulletComp->direction.x * 70, parentPos.y + bulletComp->direction.y * 70);
}

Gun::Gun(json j, GameObject* parent) : parent(parent), timer(0), nextBulletID(0)
{
	gom = &GameObjectManager::getInstance();
	gof = &GameObjectFactory::getInstance();

	int i = 0;
	for (auto bulletData : j["bulletTypes"]) {
		GameObject* bulletPrototype = gof->CreateObject(bulletData["bulletInfo"]);
		bulletPrototype->getComponent<Bullet>()->liveForever = true;
		bulletTypes[i] = std::make_pair(bulletPrototype, bulletData["fireRate"]);
	}
}

Gun::~Gun() 
{

}
