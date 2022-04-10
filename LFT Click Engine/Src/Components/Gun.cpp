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
	myTransform = componentOwner->getComponent<Transform>();
	fireRateTimer = 0;
}

void Gun::Update()
{
	fireRateTimer += g_FrameRateController->DeltaTime();
}

Component* Gun::Clone(GameObject* newParent)
{
	Gun* toReturn = new Gun();
	toReturn->componentOwner = newParent;
	toReturn->fireRateTimer = 0;
	toReturn->bulletTypes = bulletTypes;
	return toReturn;
}

bool Gun::ReadyToFire()
{
	return fireRateTimer > bulletTypes[currentBulletIndex].second;
}

void Gun::Fire(float targetX, float targetY)
{
	if (myTransform == nullptr) return;

	GameObject* bullet = gom->ClonePrefabOfTag(gof, bulletTypes[currentBulletIndex].first);
	Transform* bulletTransform = bullet->getComponent<Transform>();
	Bullet* bulletComp = bullet->getComponent<Bullet>();
	DirectX::SimpleMath::Vector2 playerPos = myTransform->CurrentPos();
	DirectX::SimpleMath::Vector2 target(targetX, targetY);
	bulletComp->direction = target - playerPos;
	bulletComp->direction.Normalize();
	//bulletComp->direction = DirectX::XMVector2Transform(bulletComp->direction, DirectX::XMMatrixRotationZ(180));
	bulletTransform->SetPos(playerPos.x + bulletComp->direction.x * 70, playerPos.y + bulletComp->direction.y * 70);

	fireRateTimer = 0;
}

void Gun::Deserialize(json j, GameObject* componentOwner)
{
	this->componentOwner = componentOwner;

	int i = 0;
	for (auto bulletData : j["bulletTypes"]) {
		bulletTypes[i] = std::make_pair(bulletData["prefabTag"], 1.f / bulletData["fireRate"]);
	}
}

void Gun::SwitchBulletIndex(int bulletIndex) {
	currentBulletIndex = bulletIndex;
}