#pragma once
// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	EnemySpawner.h
// Author			:	Rohit Punjabi
// Creation Date	:	2022/3/3
// Purpose			:	
// History			:
// ---------------------------------------------------------------------------
#include <string>
#include "Components/GameObject.h"
#include <json.hpp>

using json = nlohmann::json;

class EnemySpawner
{
public:
	EnemySpawner();
	void Update();
	void Start();

private:
	float spawnTimer;
};