// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	GameManager.h
// Author			:	Vance Howald
// Creation Date	:	2021/12/10
// Purpose			:	Animates grabables
// History			: 
// ---------------------------------------------------------------------------
#pragma once
#include "Components\Camera.h"
#include "GameObjectManager.h"

class GameManager
{
public:
	GameManager();
	~GameManager();

public:
	bool playerDead;
	bool debugMode;
	bool playerRestart;
	float playerScore;
	Camera* mainCamera;
	GameObjectManager* gom;
};

extern std::unique_ptr<GameManager> g_GameManager;