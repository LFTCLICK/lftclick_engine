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

void GameManager::UpdateTime()
{
	time += g_FrameRateController->DeltaTime();
	if (time > DAY_LENGTH) {
		time -= DAY_LENGTH;
		day++;
	}
}

float GameManager::GetDarknessLevel()
{
	if (time < SUN_SETTING) return 0;
	if (time < SUN_DOWN) return 1 - ((SUN_DOWN - time) / (SUN_DOWN - SUN_SETTING));
	if (time < SUN_RISING) return 1;
	return (DAY_LENGTH - time) / (DAY_LENGTH - SUN_RISING);
}
