#pragma once
// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	EnemySpawner.h
// Author			:	Vance Howald
// Creation Date	:	2021/12/10
// Purpose			:	Creates a region where enemies can spawn and then spawns them in waves using a linear equation
// History			: 
// ---------------------------------------------------------------------------

#include "GameObject.h"
#include "Transform.h"
#include "Component.h"
#include "GameObjectManager.h"
#include <json.hpp>

using json = nlohmann::json;
class EnemySpawner : public Component
{
public:

	// Inherited via Component
	virtual void Start() override;
	virtual void Update() override;
	virtual int getCompId() override { return ComponentType::BULLET; };
	virtual void Deserialize(nlohmann::json j, GameObject* componentOwner) override;

	virtual Component* Clone(GameObject* newParent);
	EnemySpawner() : objectSpawnName("enemy") {};

public:
	DirectX::SimpleMath::Vector2 bounds;
	DirectX::SimpleMath::Vector2 targetPosOffset;
	float slope, timeBetweenPhases, timer, c, x;

private:
	DirectX::SimpleMath::Vector2 myPos;
	std::string objectSpawnName;
};