// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	GameManager.h
// Author			:	Vance Howald
// Creation Date	:	2021/12/10
// Purpose			:	Animates grabables
// History			: 
// ---------------------------------------------------------------------------
#pragma once

constexpr auto DAY_LENGTH = 10.f;	// total length of a day;

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


constexpr auto SUN_SETTING = 2.f;	// when the light should begin lowering;
constexpr auto SUN_DOWN = 4.f;		// when the light should remain at the lowest;
constexpr auto SUN_RISING = 8.f;// when the light should begin getting brighter;


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
		mainCamera(nullptr),
		day(1), 
		time(0),
		currentLevel(EGameLevel::Mainmenu)
	{}
	~GameManager() = default;

	void UpdateTime();
	float GetDarknessLevel();

public:
	GameObject* playerObj;
	bool playerDead;
	float playerScore;
	Camera* mainCamera;
	Transform* playerTrans;

	int day;
	float time;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> menuBackgroundSRV;

	EGameLevel currentLevel;
};

extern std::unique_ptr<GameManager> g_GameManager;