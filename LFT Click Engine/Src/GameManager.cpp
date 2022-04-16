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
#include "EventManager.h"
#include "Components/Player.h"

void GameManager::LoadLevel(json file, EGameLevel toSet)
{
	g_GameObjManager->DeleteAll();
	g_GameObjManager->Deserialize(g_GameObjFactory.get(), file);
	this->currentLevel = toSet;
	this->prevLevel = toSet;

	if (toSet == EGameLevel::SideScrollerLevel)
	{
		GameObject* playerObj = g_GameObjManager->FindObjectOfTag("player");
		Player* playerComp = playerObj->getComponent<Player>();

		playerComp->collectibleparts = playerComp->collectibleWood = 0;
		playerComp->autopilot = true;

		this->playerObj = playerObj;
		this->mainCamera = playerObj->getComponent<Camera>();
		this->playerTrans = playerObj->getComponent<Transform>();
		this->playerPos = playerTrans->CurrentPos();
		
		this->time = 0;

		g_FrameRateController->zeroDeltaTime = false;
		playerDead = false;
	}
	if (toSet == EGameLevel::SurvivalLevel)
	{
		GameObject* playerObj = g_GameObjManager->FindObjectOfTag("player");
		Player* playerComp = playerObj->getComponent<Player>();

		this->playerObj = playerObj;
		this->mainCamera = playerObj->getComponent<Camera>();
		this->playerTrans = playerObj->getComponent<Transform>();
		playerComp->collectibleparts = playerComp->collectibleWood = 0;
		this->time = 0;

		g_FrameRateController->zeroDeltaTime = false;
		playerDead = false;

		PushPlayerMessage("Ugh, where am I?", 2.f);
		PushPlayerMessage("This place smells terrible.", 2.f);
		PushPlayerMessage("Who the heck would build a cabin this far out?", 2.f);

		outdoorScaryMessagePlayed = false;
	}
}

void GameManager::PushPlayerMessage(std::string message, float timeout) 
{
	messageQueue.push({ message, timeout });
}

TimedMessage GameManager::GetPlayerMessage()
{
	TimedMessage next = messageQueue.front();
	messageQueue.pop();
	return next;
}

void GameManager::Update() {
	if (!playerDead)
		playerPos = playerTrans->CurrentPos();

	UpdateCheats();
	UpdateTime();
	UpdateDanger();
	UpdateLevel();
	UpdateInsideHouse();
	UpdateSpawners();
	if (currentLevel == EGameLevel::SideScrollerLevel)
		SideScrollerObjectDestroyer();
}


void GameManager::UpdateCheats()
{
	// Cheats for wood/part gains are in Player.cpp at the end of Update().
	// The rest are here.

	if (g_InputManager->isKeyTriggered(SDL_SCANCODE_F3)) {
		if (time < SUN_DOWN)
			time = SUN_DOWN;
		else if (time < SUN_UP)
			time = SUN_UP;
		else
			time = DAY_LENGTH;
	}

	if (g_InputManager->isKeyTriggered(SDL_SCANCODE_F5)) {
		godMode = !godMode;
	}

	if (g_InputManager->isKeyTriggered(SDL_SCANCODE_F6)) {
		g_AudioManager->ToggleMuteMusic();
	}
}

#include <cmath>
void GameManager::UpdateTime()
{
	time += g_FrameRateController->DeltaTime();
	if (time > DAY_LENGTH) {
		time -= DAY_LENGTH;
		day++;
	}

	//static float w = 1880.0f;
	//static float h = 1930.0f;
	//static float x = -1000.0f;
	//static float y = -920.0f;
	//cabinRect = DirectX::SimpleMath::Rectangle(x, y, w, h);
	DirectX::SimpleMath::Rectangle playerRect = DirectX::SimpleMath::Rectangle(playerTrans->position.x, playerTrans->position.y, 
		1, 1);


	float oldDarknessLevel = darknessLevel;
	bool wasNightTime = IsNightTime();

	if (currentLevel == EGameLevel::SurvivalLevel)
	{
		if (time < SUN_DOWN) darknessLevel = 1 - ((SUN_DOWN - time) / SUN_DOWN);
		else if (time < SUN_RISING) darknessLevel = 1;
		else if (time < SUN_UP) darknessLevel = (SUN_UP - time) / (SUN_UP - SUN_RISING);
		else  darknessLevel = 0;
	}
	else if (darknessLevel != 0.1)
		darknessLevel = 0.1;

	static float fadeInTimer = 0.0f;
	static float fadeOutTimer = 0.0f;
	//player is inside cabin
	isPlayerInsideCabin = cabinRect.Intersects(playerRect);
	if (isPlayerInsideCabin)
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
	ImGui::Text("Part chance: %f", chanceOfFindingPart);

#endif // DEBUG


	if (wasNightTime && !IsNightTime()) {
		harshLightOfDay = day;
		PushPlayerMessage("Daybreak! Finally!");
		PushPlayerMessage("I saw some piles of junk outside...");
		PushPlayerMessage("Maybe I can find some motorcycle parts?");
		PushPlayerMessage("If I can repair my bike...");
		PushPlayerMessage("Maybe there's a chance I get out of here alive!");
	}

	if (!wasNightTime && IsNightTime()) {
		PushPlayerMessage("Oh no, daylight's fading...");

		if (playerInsideHouse) {
			switch (day) {
			case 1:
				PushPlayerMessage("I hope my fortifications hold...");
				PushPlayerMessage("If they break them down,");
				PushPlayerMessage("I'll have to use furnture to repair them!");
				break;
			case 2:
				PushPlayerMessage("Are there even more of them out there?");
				PushPlayerMessage("I hope the wood in this furniture is strong!");
				break;
			default:
				PushPlayerMessage("There's even more of them!");
				PushPlayerMessage("Better stay inside!");
				break;
			}
		} else {
			switch (day) {
			case 1: 
				PushPlayerMessage("Why did I stay out for so long?!", 1.5f);
				PushPlayerMessage("I better get back to the cabin, fast!", 1.5f); 
				break;
			default: 
				PushPlayerMessage("By now, I should know better than to be out for that long!"); 
				break;
			}
			PushPlayerMessage("I hope I still have time to bar the doors!", 1.5f);
		}
	}

	if (!outdoorScaryMessagePlayed && IsNightTime() && !playerInsideHouse && messageQueue.empty()) {
		PushPlayerMessage("They seem to flock to me if I go out at night!", 1.5f);
		PushPlayerMessage("I think I should stay inside right now!", 1.5f);
		outdoorScaryMessagePlayed = true;
	}
}

void GameManager::UpdateDanger()
{
	dangerLevel = (darknessLevel + (monsterCount < MAX_DANGER_ENEMY_COUNT ? (float)monsterCount / MAX_DANGER_ENEMY_COUNT : 1)) / 2.f;
}

void GameManager::UpdateLevel() 
{
	if (playerDead && currentLevel != EGameLevel::Mainmenu) {
		currentLevel = EGameLevel::Pausemenu;
	}

	if (prevFrameLevel != currentLevel) {
		if (currentLevel == EGameLevel::Pausemenu) {
			PauseLevelAudio();
			PlayMenuMusic();
		}
		if (prevFrameLevel == EGameLevel::Pausemenu) {
			UnpauseLevelAudio();
			StopMenuMusic();
		}
	}

	prevFrameLevel = currentLevel;
}

void GameManager::UpdateInsideHouse()
{
	playerInsideHouse = IsPosInsideHouse(playerPos);
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

void GameManager::SideScrollerObjectDestroyer()
{
	auto objIt = g_GameObjManager->gameObjectList.begin();

	while (objIt != g_GameObjManager->gameObjectList.end())
	{
		GameObject* toCheckObject = *objIt;
		Transform* toCheckObjectTransform = toCheckObject->getComponent<Transform>();

		if (toCheckObject->tag == "wall" || toCheckObject->tag == "bullet")
		{
			break;
		}
		else
		{
			if (playerObj == nullptr)
			{
				return;
			}
			else if (toCheckObjectTransform->position.x < playerObj->getComponent<Transform>()->position.x - 800)
			{
				toCheckObject->isDeletable = true;
			}
		}
		
		objIt++;
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
	if (chanceOfFindingPart >= INITIAL_CHANCE_TO_FIND_PART + (CHANCE_TO_FIND_PART_INCREMENT * ROLLS_TILL_PITY_PART))
		return 1.0f;
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

void GameManager::SetMenuMusic(std::string name, float volume) {
	menuMusicName = name;
	g_AudioManager->SetGroupVolume(MENU_MUSIC_MASTER_CHANNEL_GROUP, volume);
	g_AudioManager->LoadSound(menuMusicName, true, false, true);
}
void GameManager::SetButtonClickSound(std::string name, float volume) {
	buttonClickSoundName = name;
	g_AudioManager->SetGroupVolume(BUTTON_CLICK_MASTER_CHANNEL_GROUP, volume);
	g_AudioManager->LoadSound(buttonClickSoundName);
}

void GameManager::PauseLevelAudio() {
	g_AudioManager->PauseGroup(MUSIC_MASTER_CHANNEL_GROUP);
	g_AudioManager->PauseGroup(SFX_MASTER_CHANNEL_GROUP);
}
void GameManager::UnpauseLevelAudio() {
	g_AudioManager->UnpauseGroup(MUSIC_MASTER_CHANNEL_GROUP);
	g_AudioManager->UnpauseGroup(SFX_MASTER_CHANNEL_GROUP);
}
void GameManager::PlayMenuMusic() {
	g_AudioManager->PlaySound(menuMusicName, MENU_MUSIC_MASTER_CHANNEL_GROUP);
	g_AudioManager->SetGroupSpatialPosition(MENU_MUSIC_MASTER_CHANNEL_GROUP, playerPos);
}
void GameManager::StopMenuMusic() {
	g_AudioManager->StopGroup(MENU_MUSIC_MASTER_CHANNEL_GROUP);
}
void GameManager::PlayButtonClick() {
	if (buttonClickSoundName != "none") {
		g_AudioManager->PlaySound(buttonClickSoundName, BUTTON_CLICK_MASTER_CHANNEL_GROUP);
		g_AudioManager->SetGroupSpatialPosition(BUTTON_CLICK_MASTER_CHANNEL_GROUP, playerPos);
	}
}