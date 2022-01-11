// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	GameManager.h
// Author			:	Vance Howald
// Creation Date	:	2021/12/10
// Purpose			:	Animates grabables
// History			: 
// ---------------------------------------------------------------------------
#pragma once
#include ".//Components/Camera.h"
#include "Components/GameObjectManager.h"

class GameManager
{
public:
	GameManager();


	static GameManager& getInstance()
	{
		static GameManager instance;
		return instance;
	}
	~GameManager();

public:
	bool playerDead;
	bool playerRestart;
	float playerScore;
	Camera* mainCamera;
	GameObjectManager* gom;
};