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

#include <queue>
#include <json.hpp>
#include <Helpers.h>

//#define SUN_SETTING 73.2473f		// when the light should begin getting brighter
//#define SUN_DOWN 125.3655f		// when the light should remain at the brightest
//#define SUN_RISING 177.4838f		// when the light should begin lowering
//#define SUN_UP 262.f				// when the light should remain at the lowest

#define SUN_RISING 156.965f			// when the light should begin getting brighter (230.49 on audio track)
#define SUN_UP 172.525f				// total length of a day (246.05 on audio track)
#define SUN_SETTING 199.792f		// when the light should begin lowering
#define SUN_DOWN 262.595f			// when the light should remain at the lowest

#define DAY_LENGTH 262.595f			// when the counter will flip back to zero


#define INITIAL_CHANCE_TO_FIND_PART 0.05f
#define CHANCE_TO_FIND_PART_INCREMENT 0.0125f
#define ROLLS_TILL_PITY_PART 13

#define MAX_DANGER_ENEMY_COUNT 100.f

#define DAY_NIGHT_THRESHOLD 0.5f

#define HOUSE_POS DirectX::SimpleMath::Vector2(0, 0)
#define HOUSE_SCALE DirectX::SimpleMath::Vector2(1920, 1920)

// How frequently (roughly) enemies will spawn during the two different times of day, in seconds.
// The min/max is for randomization of how long the spawn takes to go off.
#define DAYTIME_SPAWN_INTERVAL_MIN 4.0f
#define DAYTIME_SPAWN_INTERVAL_MAX 6.0f
#define NIGHTTIME_SPAWN_INTERVAL_MIN 2.0f
#define NIGHTTIME_SPAWN_INTERVAL_MAX 4.0f
#define NIGHTTIME_SPAWN_INTERVAL_OUTSIDE_MIN  NIGHTTIME_SPAWN_INTERVAL_MIN / 4
#define NIGHTTIME_SPAWN_INTERVAL_OUTSIDE_MAX  NIGHTTIME_SPAWN_INTERVAL_MAX / 4

// The mod-per-day values are added to the intervals, and are multiplied by how many days have gone by.
#define DAYTIME_SPAWN_MOD_PER_DAY -0.5f
#define NIGHTTIME_SPAWN_MOD_PER_DAY -0.3f

#define MIN_TIME_BETWEEN_SPAWNS 0.5f
#define MAX_ENEMIES 500

#define NEARPLAYER_ENEMY_SPAWNER_ID 1000

#define MESSAGE_POSSIBILITIES 5


class Camera;
class Transform;
class GameObject;



enum class EGameLevel
{
	Intro,
	Mainmenu,
	Pausemenu,
	Level0,
	CreditsScreen,
	ControlScreen
};

struct TimedMessage 
{
	std::string message;
	float timeout;
};

class GameManager
{
public:
	GameManager() : 
		playerObj(nullptr),
		playerDead(false), 
		playerWon(false), 
		displayDarknessLevel(0),
		darknessLevel(1),
		monsterCount(0),
		dangerLevel(0),
		rednessFactor(0),
		fadFactor(0),
		mapHeight(10000.0f),
		mainCamera(nullptr),
		day(1), 
		time(0),
		chanceOfFindingPart(INITIAL_CHANCE_TO_FIND_PART),
		currentLevel(EGameLevel::Mainmenu),
		harshLightOfDay(day-1),
		totalSpawners(0),
		activatedSpawner(-1),
		spawnTimer(0),
		spawnIntervalMin(NIGHTTIME_SPAWN_INTERVAL_MIN),
		spawnIntervalMax(NIGHTTIME_SPAWN_INTERVAL_MAX)
	{}
	~GameManager() = default;

	// Update calls all Update functions.
	void Update();

	void UpdateTime();
	void UpdateDanger();
	void UpdateLevel();
	void UpdateInsideHouse();
	void UpdateSpawners();

	// Returns value between 0 and 1, 1 being night time and 0 being day time.
	// Used for darkness overlay alpha.
	float GetDarknessLevel();

	// Returns true if "darknessLevel" is over DAY_NIGHT_THRESHOLD, false otherwise.
	bool IsNightTime();

	// Returns value between 0 and 100, 0 being minimal danger, 100 being maximum.
	// Calculated using level of darkness and number of zombies.
	// Used to modify background music volume.
	float GetDangerLevel();

	// If the spawner's number comes up, it will be activated.
	// Resets "activatedSpawner" to -1 when spawnerID is correct.
	bool IsSpawnerActivated(int spawnerID);

	// Returns value between 0 and 1. 
	// 0 being no chance of finding a motorcycle part, 1 being 100% chance.
	float GetChanceOfFindingPart();

	// Resets 'chanceOfFindingPart' to 'INITIAL_CHANCE_TO_FIND_PART'
	void PartSearchFailed();

	// Increments 'chanceOfFindingPart' by 'CHANCE_TO_FIND_PART_INCREMENT'
	void PartSearchSuccessful();

	void MonsterCountPlus();
	void MonsterCountMinus();

	bool IsPosInsideHouse(DirectX::SimpleMath::Vector2 pos);

	void LoadLevel(nlohmann::json file, EGameLevel toSet);

	void PushPlayerMessage(std::string, float timeout = 3.f);
	TimedMessage GetPlayerMessage();

public:
	GameObject* playerObj;
	bool playerDead;
	bool playerWon;
	bool playerInsideHouse;
	float darknessLevel;
	float rednessFactor;
	float fadFactor;
	float displayDarknessLevel;
	int monsterCount;
	int harshLightOfDay;
	int windowWidth;
	int windowHeight;
	float dangerLevel;
	float mapHeight;
	float spawnTimer;
	float spawnIntervalMin;
	float spawnIntervalMax;
	Camera* mainCamera;
	Transform* playerTrans;

	DirectX::SimpleMath::Rectangle cabinRect = DirectX::SimpleMath::Rectangle(-1000.0f, -920.0f, 1880.0f, 1930.0f);

	int day;
	float time;

	float chanceOfFindingPart;

	int totalSpawners;
	int activatedSpawner;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> menuBackgroundSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> creditsSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> controlsSRV;

	EGameLevel currentLevel;
	EGameLevel prevLevel;

	std::queue<TimedMessage> messageQueue;

	//std::queue<std::string> daytimeSwitchMessages[MESSAGE_POSSIBILITIES] 
};

extern std::unique_ptr<GameManager> g_GameManager;