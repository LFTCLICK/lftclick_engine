#include "EnemySpawner.h"
#include "FrameRateController.h"
#include "pch.h"
#include <json.hpp>
#include <string>
#include <fstream>
#include <iostream>


EnemySpawner::EnemySpawner() : spawnTimer(5.0)
{

}

void EnemySpawner::Update()
{
	spawnTimer -= FrameRateController::getInstance().DeltaTime();

	if (spawnTimer < 0.0)
	{

	}
}

void EnemySpawner::Start()
{

}
