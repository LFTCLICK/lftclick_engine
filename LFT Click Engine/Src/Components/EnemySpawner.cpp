#include "pch.h"
#include "EnemySpawner.h"
#include "./FrameRateController.h"
#include "Enemy.h"
#include "GameManager.h"

void EnemySpawner::Start()
{
	positiveBounds = Vector2(1280, 7680);
	negativeBounds = Vector2(-1280, -7680);
	spawnTimer = 0;
	trans = componentOwner->getComponent<Transform>();
	spawnerID = spawnAroundPlayer ? NEARPLAYER_ENEMY_SPAWNER_ID : g_GameManager->totalSpawners++;
}

void EnemySpawner::Update()
{
	spawnTimer -= g_FrameRateController->DeltaTime();
	
	if (spawnTimer <= 0)
	{
		spawnTimer = 3;
		DirectX::SimpleMath::Vector2 spawnPos;

		spawnPos = componentOwner->trans->CurrentPos();
		int spawnChanceRes = Helpers::randWithinRange(0, totalSpawnChance);

		for (auto typeIt = enemyTypes.begin(); typeIt != enemyTypes.end(); typeIt++)
		{
			if (spawnChanceRes <= typeIt->first)
			{
				g_GameObjManager->ClonePrefabOfTag(g_GameObjFactory.get(), typeIt->second)->getComponent<Transform>()->SetPos(spawnPos.x, spawnPos.y);
				break;
			}
		}
	}

	//if (g_GameManager->IsSpawnerActivated(spawnerID)) 
	//{
	//	DirectX::SimpleMath::Vector2 spawnPos;

	//	if (spawnAroundPlayer && !g_GameManager->playerInsideHouse) {
	//		DirectX::SimpleMath::Vector2 playerPos = g_GameManager->playerTrans->CurrentPos();
	//		int halfWindowHeight = g_GameManager->windowHeight / 2, halfWindowWidth = g_GameManager->windowWidth / 2;

	//		int randInt = Helpers::randWithinRange(0, 3);
	//		int horizontalOrVertical = (randInt / 2 == 0), signMod = (randInt % 2 == 0 ? 1 : -1);

	//		if (horizontalOrVertical) {
	//			float distanceFromWindow = playerPos.x + (signMod * (halfWindowWidth + NEARPLAYER_SPAWN_DISTANCE_FROM_WINDOW));
	//			spawnPos = {
	//				Helpers::randWithinRange(distanceFromWindow, distanceFromWindow + signMod * NEARPLAYER_SPAWN_AREA_WIDTH),
	//				Helpers::randWithinRange(playerPos.y - halfWindowHeight, playerPos.y + halfWindowHeight)
	//			};
	//		}
	//		else {
	//			float distanceFromWindow = playerPos.y + (signMod * (halfWindowHeight + NEARPLAYER_SPAWN_DISTANCE_FROM_WINDOW));
	//			spawnPos = {
	//				Helpers::randWithinRange(playerPos.x - halfWindowWidth, playerPos.x + halfWindowWidth),
	//				Helpers::randWithinRange(distanceFromWindow, distanceFromWindow + signMod * NEARPLAYER_SPAWN_AREA_WIDTH)
	//			};
	//		}
	//	}
	//	else if (!spawnAroundPlayer) {
	//		DirectX::SimpleMath::Vector2 currentPos = trans->CurrentPos();
	//		float halfScaleWidth = trans->scale.x / 2, halfScaleHeight = trans->scale.y / 2;
	//		spawnPos = {
	//			Helpers::randWithinRange(currentPos.x - halfScaleWidth, currentPos.x + halfScaleWidth),
	//			Helpers::randWithinRange(currentPos.y - halfScaleHeight, currentPos.y + halfScaleHeight)
	//		};
	//	}
	//	else spawnPos = { 0, 0 };

	//	if (!g_GameManager->IsPosInsideHouse(spawnPos)) {
	//		int spawnChanceRes = Helpers::randWithinRange(0, totalSpawnChance);
	//		for (auto typeIt = enemyTypes.begin(); typeIt != enemyTypes.end(); typeIt++)
	//		{
	//			if (spawnChanceRes <= typeIt->first)
	//			{
	//				g_GameObjManager->ClonePrefabOfTag(g_GameObjFactory.get(), typeIt->second)->getComponent<Transform>()->SetPos(spawnPos.x, spawnPos.y);
	//				break;
	//			}
	//		}
	//	}
	//}

	//// past this is going unused as of now
	//if (spawnTimer <= 0.0f)
	//{
	//	spawnTimer = timeBetweenPhases;

	//	if (objectSpawnName == "zombie")
	//	{
	//		int toSpawn;
	//		// checking if its morning time
	//		if (g_GameManager->IsNightTime())
	//			toSpawn = 30.0f;
	//		else
	//			toSpawn = 10.0f;

	//		totalZombiesOnMap += toSpawn;
	//		for (int i = 0; i < toSpawn; i++)
	//		{
	//			GameObject* newObj = g_GameObjManager->ClonePrefabOfTag(g_GameObjFactory.get(), objectSpawnName);
	//			newObj->getComponent<Transform>()->SetPos(randomNumberGetter(), randomNumberGetter());
	//		}
	//		printf("Total zombies on map are %d\n", totalZombiesOnMap);
	//	}
	//	
	//	if (objectSpawnName == "enemy")
	//	{
	//		ghostsOnMap += 1;
	//		GameObject* newObj = g_GameObjManager->ClonePrefabOfTag(g_GameObjFactory.get(), objectSpawnName);
	//		newObj->getComponent<Transform>()->SetPos(randomNumberGetter(), randomNumberGetter());
	//		printf("Total ghosts on map are %d\n", ghostsOnMap);
	//	}

	//	++x;
	//}
}

void EnemySpawner::Deserialize(nlohmann::json j, GameObject* componentOwner)
{
	this->componentOwner = componentOwner;

	if (j.contains("slope")) slope = j["slope"];
	if (j.contains("c")) c = j["c"];
	if (j.contains("timeBetweenPhases")) timeBetweenPhases = j["timeBetweenPhases"];
	if (j.contains("spawnAroundPlayer")) spawnAroundPlayer = j["spawnAroundPlayer"];
	if (j.contains("objectSpawnName")) objectSpawnName = j["objectSpawnName"];


	if (j.contains("enemyTypes")) {
		for (auto enemyTypeIt = std::begin(j["enemyTypes"]); enemyTypeIt != std::end(j["enemyTypes"]); enemyTypeIt++) {
			totalSpawnChance += (*enemyTypeIt)["spawnChance"];
			enemyTypes[totalSpawnChance] = (*enemyTypeIt)["name"];
		}
	}
}

float EnemySpawner::randomNumberGetter()
{
	int value = rand() % 2;
	if (value == 0) return positiveBounds.x + static_cast <float> (std::rand() / (static_cast <float> (RAND_MAX / (positiveBounds.y - positiveBounds.x))));
	else if (value == 1)
		return negativeBounds.y + static_cast <float> (std::rand() / (static_cast <float> (RAND_MAX / (negativeBounds.x - negativeBounds.y))));
}

Component* EnemySpawner::Clone(GameObject* newParent)
{
	EnemySpawner* toReturn = new EnemySpawner();
	toReturn->slope = slope;
	toReturn->c = c;
	toReturn->timeBetweenPhases = timeBetweenPhases;
	toReturn->objectSpawnName = objectSpawnName;
	toReturn->totalSpawnChance = totalSpawnChance;
	toReturn->enemyTypes = enemyTypes;
	toReturn->spawnAroundPlayer = spawnAroundPlayer;
	toReturn->componentOwner = newParent;
	return toReturn;
}