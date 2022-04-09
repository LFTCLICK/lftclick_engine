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
	GameObject* bullet = gom->ClonePrefabOfTag(gof, bulletTypes[bulletIndex].first);
	Transform* bulletTransform = bullet->getComponent<Transform>();
	Bullet* bulletComp = bullet->getComponent<Bullet>();
	DirectX::SimpleMath::Vector2 parentPos = trans->CurrentPos();

	DirectX::SimpleMath::Vector2 target(targetX, targetY);
	bulletComp->direction = target - parentPos;
	bulletComp->direction.Normalize();
	bulletTransform->SetPos(parentPos.x + bulletComp->direction.x * 70, parentPos.y + bulletComp->direction.y * 70);
}

void Gun::FireSine(int bulletIndex, float targetX, float targetY)
{
	GameObject* bullet = gom->ClonePrefabOfTag(gof, bulletTypes[bulletIndex].first);
	Transform* bulletTransform = bullet->getComponent<Transform>();
	Bullet* bulletComp = bullet->getComponent<Bullet>();
	DirectX::SimpleMath::Vector2 parentPos = trans->CurrentPos();

	DirectX::SimpleMath::Vector2 target(targetX, targetY);
	bulletComp->direction.x = target.x - parentPos.x + (50 * sin(2 * 3.142 * 12 * 15 + 5));;
	bulletComp->direction.y = target.y - parentPos.y;
	bulletComp->direction.Normalize();
	bulletTransform->SetPos(parentPos.x + bulletComp->direction.x * 70, parentPos.y + bulletComp->direction.y * 70);
}

Gun::Gun()
{
	gom = &GameObjectManager::getInstance();
	gof = &GameObjectFactory::getInstance();
}

void Gun::Deserialize(json j, GameObject* parent)
{
	this->parent = parent;
	timer = 0;
	nextBulletID = 0;

	int i = 0;
	for (auto bulletData : j["bulletTypes"]) {
		bulletTypes[i] = std::make_pair(bulletData["prefabTag"], bulletData["fireRate"]);
	}
}

Gun::~Gun() 
{
}
