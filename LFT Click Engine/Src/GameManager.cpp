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

//probably should be removed 
GameManager::GameManager()
{
	playerDead = false;
	debugMode = false;
	playerScore = 0;
	playerRestart = false;
}

GameManager::~GameManager()
{
}
