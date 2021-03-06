#pragma once
// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	EnemySpawner.h
// Author			:	Rohit Punjabi
// Creation Date	:	2022/03/27
// Purpose			:	Selects a random position outside of house to spawn enemies.
//						The number of enemies to spawn is decided on the day cycle.
// History			: 
// ---------------------------------------------------------------------------

#include "GameObject.h"
#include "Transform.h"
#include "Component.h"
#include "Helpers.h"
#include "GameObjectManager.h"
#include <json.hpp>

#define NEARPLAYER_SPAWN_DISTANCE_FROM_WINDOW 240.f
#define NEARPLAYER_SPAWN_AREA_WIDTH 240.f

using namespace DirectX::SimpleMath;
using json = nlohmann::json;
class EnemySpawner : public Component
{
public:

	// Inherited via Component
	virtual void Start() override;
	virtual void Update() override;
	virtual int getCompId() override { return ComponentType::ENEMY_SPAWNER; };
	static int getStaticCompId() { return ComponentType::ENEMY_SPAWNER; };
	virtual void Deserialize(nlohmann::json j, GameObject* componentOwner) override;

	float randomNumberGetter();

	virtual Component* Clone(GameObject* newParent);
	EnemySpawner() : objectSpawnName("enemy"), timeBetweenPhases(10.f), spawnAroundPlayer(false), totalSpawnChance(0) {};

public:
	Vector2 bounds;
	Vector2 targetPosOffset;
	float slope, timeBetweenPhases, c, x, spawnTimer;
	int totalZombiesOnMap, ghostsOnMap;
	bool spawnAroundPlayer;

private:
	Transform* myTransform;

	int spawnerID;
	Vector2 myPos;
	Vector2 positiveBounds, negativeBounds;
	std::string objectSpawnName;

	int totalSpawnChance;
	std::map<int, std::string> enemyTypes;
};