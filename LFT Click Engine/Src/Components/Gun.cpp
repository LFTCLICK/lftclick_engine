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
	trans = componentOwner->getComponent<Transform>();
	timer = 0;
}

void Gun::Update()
{
	timer += g_FrameRateController->DeltaTime();
}

Component* Gun::Clone(GameObject* newParent)
{
	Gun* toReturn = new Gun();
	toReturn->componentOwner = newParent;
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

Gun::Gun()
{
	gom = g_GameObjManager.get();
	gof = g_GameObjFactory.get();
}

void Gun::Deserialize(json j, GameObject* componentOwner)
{
	this->componentOwner = componentOwner;
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
