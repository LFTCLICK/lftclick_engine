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

#include <cmath>
void GameManager::UpdateTime()
{
	time += g_FrameRateController->DeltaTime();
	if (time > DAY_LENGTH) {
		time -= DAY_LENGTH;
		day++;
	}

	static float w = 1880.0f;
	static float h = 1930.0f;
	static float x = -1000.0f;
	static float y = -920.0f;
	ImGui::DragFloat("width", &w, 10.0f, 1500, 2500.0f);
	ImGui::DragFloat("height", &h, 10.0f, 1500, 2500.0f);
	ImGui::DragFloat("x", &x, 10.0f, -3000, 3000.0f);
	ImGui::DragFloat("y", &y, 10.0f, -3000, 3000.0f);
	cabinRect = DirectX::SimpleMath::Rectangle(x, y, w, h);
	DirectX::SimpleMath::Rectangle playerRect = DirectX::SimpleMath::Rectangle(playerTrans->position.x, playerTrans->position.y, 
		1, 1);


	float oldDarknessLevel = darknessLevel;


	if (time < SUN_RISING) darknessLevel = 1;
	else if (time < SUN_UP) darknessLevel = (SUN_UP - time) / (SUN_UP - SUN_RISING);
	else if (time < SUN_SETTING) darknessLevel = 0;
	else darknessLevel = 1 - ((SUN_DOWN - time) / (SUN_DOWN - SUN_SETTING));

	static float fadeInTimer = 0.0f;
	static float fadeOutTimer = 0.0f;
	//player is inside cabin
	if (cabinRect.Intersects(playerRect))
	{
		fadeOutTimer = 0.0f;
		fadeInTimer += g_FrameRateController->DeltaTime();
		displayDarknessLevel = std::lerp(darknessLevel, 0.0f, std::clamp(fadeInTimer / 3.0f, 0.0f, 1.0f));
	}
	else
	{
		fadeInTimer = 0.0f;
		fadeOutTimer += g_FrameRateController->DeltaTime();

		displayDarknessLevel = std::lerp(displayDarknessLevel, darknessLevel, std::clamp(fadeOutTimer / 3.0f, 0.0f, 1.0f));
	}

#ifdef _DEBUG
	ImGui::DragFloat("Darkness", &darknessLevel, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("Display Darkness", &displayDarknessLevel, 0.01f, 0.0f, 1.0f);

#endif // DEBUG


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
		bool nightTime = IsNightTime();

		int randInt = rand();
		float randFloat = (float)randInt / RAND_MAX;
		float minSpawnInterval = 0, maxSpawnInterval = 0;

		float spawnMod = (nightTime ? NIGHTTIME_SPAWN_MOD_PER_DAY : DAYTIME_SPAWN_MOD_PER_DAY) * (day - 1);;

		if (playerInsideHouse) {
			activatedSpawner = randInt % totalSpawners;

			minSpawnInterval = nightTime ? NIGHTTIME_SPAWN_INTERVAL_MIN : DAYTIME_SPAWN_INTERVAL_MIN;
			maxSpawnInterval = nightTime ? NIGHTTIME_SPAWN_INTERVAL_MAX : DAYTIME_SPAWN_INTERVAL_MAX;
		}
		else {
			activatedSpawner = NEARPLAYER_ENEMY_SPAWNER_ID;

			minSpawnInterval = nightTime ? NIGHTTIME_SPAWN_INTERVAL_OUTSIDE_MIN : DAYTIME_SPAWN_INTERVAL_MIN;
			maxSpawnInterval = nightTime ? NIGHTTIME_SPAWN_INTERVAL_OUTSIDE_MAX : DAYTIME_SPAWN_INTERVAL_MAX;
		}

		spawnTimer = Helpers::randWithinRange(minSpawnInterval, maxSpawnInterval) + spawnMod;

		if (spawnTimer < MIN_TIME_BETWEEN_SPAWNS) 
			spawnTimer = MIN_TIME_BETWEEN_SPAWNS;
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