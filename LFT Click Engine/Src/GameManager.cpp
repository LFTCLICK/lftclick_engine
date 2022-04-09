// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	GameManager.cpp
// Author			:	Vance Howald
// Creation Date	:	2021/12/10
// Purpose			:	Global values for game management
// History			:
// ---------------------------------------------------------------------------
#include "pch.h"
#include "GameManager.h"
#include "FrameRateController.h"
#include "Renderer.h"
#include "GameObjectManager.h"

void GameManager::LoadLevel(json file)
{
	g_GameObjManager->DeleteAll();
	g_GameObjManager->Deserialize(g_GameObjFactory.get(), file);

	GameObject* playerObj = g_GameObjManager->FindObjectOfTag("player");
	this->playerObj = playerObj;
	this->mainCamera = playerObj->getComponent<Camera>();
	this->playerTrans = playerObj->getComponent<Transform>();
	this->time = 0;

	this->currentLevel = EGameLevel::Level0;
	g_FrameRateController->zeroDeltaTime = false;

	playerDead = false;
}

void GameManager::Update() {
	UpdateTime();
	UpdateDanger();
	UpdateLevel();
	UpdateInsideHouse();
	UpdateSpawners();
}

void GameManager::UpdateTime()
{
	time += g_FrameRateController->DeltaTime();
	if (time > DAY_LENGTH) {
		time -= DAY_LENGTH;
		day++;
	}

	float oldDarknessLevel = darknessLevel;

	//if (time < SUN_RISING) darknessLevel = 1;
	//else if (time < SUN_UP) darknessLevel = (SUN_UP - time) / (SUN_UP - SUN_RISING);
	//else if (time < SUN_SETTING) darknessLevel = 0;
	//else darknessLevel = 1 - ((SUN_DOWN - time) / (SUN_DOWN - SUN_SETTING));


	/// Uncomment it for original game
	/*if (time < SUN_RISING) darknessLevel = 1;
	else if (time < SUN_UP) darknessLevel = (SUN_UP - time) / (SUN_UP - SUN_RISING);
	else if (time < SUN_SETTING) darknessLevel = 0;
	else darknessLevel = 1 - ((SUN_DOWN - time) / (SUN_DOWN - SUN_SETTING));*/

	ImGui::DragFloat("Darkness", &darknessLevel, 0.01f, 0.0f, 1.0f);

	if (oldDarknessLevel >= DAY_NIGHT_THRESHOLD && !IsNightTime() && harshLightOfDay != day) {
		harshLightOfDay = day;
	}
}

void GameManager::UpdateDanger()
{
	dangerLevel = (darknessLevel + (monsterCount < MAX_DANGER_ENEMY_COUNT ? (float)monsterCount / MAX_DANGER_ENEMY_COUNT : 1)) / 2.f;
}

void GameManager::UpdateLevel() 
{
	if (playerDead && currentLevel != EGameLevel::Mainmenu)
		currentLevel = EGameLevel::Pausemenu;
}

void GameManager::UpdateInsideHouse()
{
	playerInsideHouse = IsPosInsideHouse(playerTrans->CurrentPos());
}

void GameManager::UpdateSpawners()
{
	spawnTimer -= g_FrameRateController->DeltaTime();
	if (spawnTimer < 0) {
		if (playerInsideHouse) {
			int randInt = rand();
			float randFloat = (float)randInt / RAND_MAX;
			activatedSpawner = randInt % totalSpawners;
			spawnTimer = IsNightTime() ?
				NIGHTTIME_SPAWN_INTERVAL_MIN + ((NIGHTTIME_SPAWN_INTERVAL_MAX - NIGHTTIME_SPAWN_INTERVAL_MIN) * randFloat) + (NIGHTTIME_SPAWN_MOD_PER_DAY * (day - 1)) :
				DAYTIME_SPAWN_INTERVAL_MIN + ((DAYTIME_SPAWN_INTERVAL_MAX - DAYTIME_SPAWN_INTERVAL_MIN) * randFloat) + (DAYTIME_SPAWN_MOD_PER_DAY * (day - 1));
		}
		else {
			float randFloat = (float)rand() / RAND_MAX;
			activatedSpawner = NEARPLAYER_ENEMY_SPAWNER_ID;
			spawnTimer = IsNightTime() ?
				NIGHTTIME_SPAWN_INTERVAL_OUTSIDE_MIN + ((NIGHTTIME_SPAWN_INTERVAL_OUTSIDE_MAX - NIGHTTIME_SPAWN_INTERVAL_OUTSIDE_MIN) * randFloat) + (NIGHTTIME_SPAWN_MOD_PER_DAY * (day - 1)) :
				DAYTIME_SPAWN_INTERVAL_MIN + ((DAYTIME_SPAWN_INTERVAL_MAX - DAYTIME_SPAWN_INTERVAL_MIN) * randFloat) + (DAYTIME_SPAWN_MOD_PER_DAY * (day - 1));
		}

		if (spawnTimer < 0) spawnTimer = MIN_TIME_BETWEEN_SPAWNS;
	}
}

float GameManager::GetDarknessLevel()
{
	return darknessLevel;
}

bool GameManager::IsNightTime()
{
	return darknessLevel > DAY_NIGHT_THRESHOLD;
}

float GameManager::GetDangerLevel()
{
	return dangerLevel;
}

bool GameManager::IsSpawnerActivated(int spawnerID)
{
	if (activatedSpawner > -1) 
	{
		if (activatedSpawner == spawnerID)
		{
			activatedSpawner = -1;
			return true;
		}
	}
	return false;
}

float GameManager::GetChanceOfFindingPart()
{
	return chanceOfFindingPart;
}

void GameManager::PartSearchFailed()
{
	chanceOfFindingPart += CHANCE_TO_FIND_PART_INCREMENT;
}

void GameManager::PartSearchSuccessful()
{
	chanceOfFindingPart = INITIAL_CHANCE_TO_FIND_PART;
}

void GameManager::MonsterCountPlus() 
{
	monsterCount++;
}

void GameManager::MonsterCountMinus()
{
	if (monsterCount > 0) monsterCount--;
}

bool GameManager::IsPosInsideHouse(DirectX::SimpleMath::Vector2 pos)
{
	auto halfHouseScale = HOUSE_SCALE / 2;

	return pos.x < HOUSE_POS.x + halfHouseScale.x &&
		pos.x > HOUSE_POS.x - halfHouseScale.x &&
		pos.y < HOUSE_POS.y + halfHouseScale.y &&
		pos.y > HOUSE_POS.y - halfHouseScale.y;
}