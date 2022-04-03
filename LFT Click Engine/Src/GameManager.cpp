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

void GameManager::UpdateTime()
{
	time += g_FrameRateController->DeltaTime();
	if (time > DAY_LENGTH) {
		time -= DAY_LENGTH;
		day++;
	}

	if (time < SUN_SETTING) darknessLevel = 0;
	else if (time < SUN_DOWN) darknessLevel = 1 - ((SUN_DOWN - time) / (SUN_DOWN - SUN_SETTING));
	else if (time < SUN_RISING) darknessLevel = 1;
	else darknessLevel = (DAY_LENGTH - time) / (DAY_LENGTH - SUN_RISING);

	dangerLevel = (darknessLevel + (monsterCount < MAX_DANGER_ENEMY_COUNT ? (float)monsterCount / MAX_DANGER_ENEMY_COUNT : 1)) / 2.f;

	if (playerDead && currentLevel != EGameLevel::Mainmenu)
	{
		currentLevel = EGameLevel::Pausemenu;
	}
}

float GameManager::GetDarknessLevel()
{
	return darknessLevel;
}

float GameManager::GetDangerLevel()
{
	return dangerLevel;
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