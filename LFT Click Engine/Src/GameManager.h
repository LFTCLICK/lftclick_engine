// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	GameManager.h
// Author			:	Vance Howald
// Creation Date	:	2021/12/10
// Purpose			:	Animates grabables
// History			: 
// ---------------------------------------------------------------------------
#pragma once

// The idea is that when "time" is: 
// 
// Between 0 and SUN_SETTING, the alpha on the darkness will remain at 0
// Between SUN_SETTING and SUN_DOWN, the alpha on the darkness will slowly increase to 1
// Between SUN_DOWN and SUN_RISING, the alpha on the darkness will remain at 1
// Between SUN_RISING and DAY_LENGTH, the alpha on the darkness will slowly decrease to 0
// 
// When the "time" reaches DAY_LENGTH, it will reset to 0 and increment the "day"
//
// The darkness alpha level is produced by "GetDarknessLevel()".

#include <json.hpp>

//#define SUN_RISING 73.2473f		// when the light should begin getting brighter
//#define SUN_UP 125.3655f			// when the light should remain at the brightest
//#define SUN_SETTING 177.4838f		// when the light should begin lowering
//#define SUN_DOWN 262.f			// when the light should remain at the lowest

#define SUN_RISING 156.965f			// when the light should begin getting brighter (230.49 on audio track)
#define SUN_UP 172.525f				// total length of a day (246.05 on audio track)
#define SUN_SETTING 199.792f		// when the light should begin lowering
#define SUN_DOWN 262.595f			// when the light should remain at the lowest

#define DAY_LENGTH 262.595f			// when the counter will flip back to zero


#define INITIAL_CHANCE_TO_FIND_PART 0.05f
#define CHANCE_TO_FIND_PART_INCREMENT 0.05f

#define MAX_DANGER_ENEMY_COUNT 100.f


class Camera;
class Transform;
class GameObject;

enum class EGameLevel
{
	Mainmenu,
	Pausemenu,
	Level0
};

class GameManager
{
public:
	GameManager() : 
		playerObj(nullptr),
		playerDead(false), 
		darknessLevel(0),
		monsterCount(0),
		dangerLevel(0),
		mapHeight(10000.0f),
		mainCamera(nullptr),
		day(1), 
		time(0),
		chanceOfFindingPart(INITIAL_CHANCE_TO_FIND_PART),
		currentLevel(EGameLevel::Mainmenu)
	{}
	~GameManager() = default;

	void UpdateTime();

	// Returns value between 0 and 1, 1 being night time and 0 being day time.
	// Used for darkness overlay alpha.
	float GetDarknessLevel();

	// Returns true if "darknessLevel" is over 0.9, false otherwise.
	bool IsNightTime();

	// Returns value between 0 and 100, 0 being minimal danger, 100 being maximum.
	// Calculated using level of darkness and number of zombies.
	// Used to modify background music volume.
	float GetDangerLevel();

	// Returns value between 0 and 1. 
	// 0 being no chance of finding a motorcycle part, 1 being 100% chance.
	float GetChanceOfFindingPart();

	// Resets 'chanceOfFindingPart' to 'INITIAL_CHANCE_TO_FIND_PART'
	void PartSearchFailed();

	// Increments 'chanceOfFindingPart' by 'CHANCE_TO_FIND_PART_INCREMENT'
	void PartSearchSuccessful();

	void MonsterCountPlus();
	void MonsterCountMinus();


	void LoadLevel(nlohmann::json file);
public:
	GameObject* playerObj;
	bool playerDead;
	float darknessLevel;
	int monsterCount;
	float dangerLevel;
	float mapHeight;
	Camera* mainCamera;
	Transform* playerTrans;

	int day;
	float time;

	float chanceOfFindingPart;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> menuBackgroundSRV;

	EGameLevel currentLevel;
};

extern std::unique_ptr<GameManager> g_GameManager;