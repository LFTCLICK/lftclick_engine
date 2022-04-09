#pragma once
// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	EnemySpawner.h
// Author			:	Rohit Punjabi
// Creation Date	:	2022/3/3
// Purpose			:	
// History			:
// ---------------------------------------------------------------------------
#include "Components/GameObject.h"
#include "GameObjectFactory.h"
#include "GameObjectManager.h"
#include "Components/GameObject.h"
#include "Components/Enemy.h"

class EnemySpawner
{
public:
	EnemySpawner();
	void Update();
	void Start();

private:
	GameObjectManager* gom;
	GameObjectFactory* gof;
	Enemy* enemy;

	float spawnTimer;
	std::vector<DirectX::SimpleMath::Vector2> spawnLocations;
};