// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	GameManager.h
// Author			:	Vance Howald
// Creation Date	:	2021/12/10
// Purpose			:	Animates grabables
// History			: 
// ---------------------------------------------------------------------------
#pragma once

#define DAY_LENGTH 10.f	// total length of a day

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


#define SUN_SETTING 73.2473118279552f	// when the light should begin lowering
#define SUN_DOWN 125.3655913978464f		// when the light should remain at the lowest
#define SUN_RISING 177.4838709677376f	// when the light should begin getting brighter
#define DAY_LENGTH 262.f				// total length of a day


#define INITIAL_CHANCE_TO_FIND_PART 0.05f
#define CHANCE_TO_FIND_PART_INCREMENT 0.05f

#define MAX_DANGER_ENEMY_COUNT 100.f


class Camera;
class Transform;
class GameObject;

enum class EGameLevel
{
	Mainmenu,
	Level0
};

class GameManager
{
public:
	GameManager() : 
		playerObj(nullptr),
		playerDead(false), 
		playerScore(0), 
		darknessLevel(0),
		monsterCount(0),
		dangerLevel(0),
		playerRestart(false),
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

public:
	GameObject* playerObj;
	bool playerDead;
	bool playerRestart;
	float playerScore;
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