// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	FrameRateControler.cpp
// Author			:	Vance Howald
// Creation Date	:	2021/10/06
// Purpose			:	Implementation
// History			:
// ---------------------------------------------------------------------------
#include "FrameRateControler.h"
#include <iostream>

void FrameRateControler::Init(Uint32 ticksPerFrame)
{
	frameTimeTicks = ticksPerFrame;
	startOfFrameTime = 0;
	ticksSinceLastFrame = 0;
}

void FrameRateControler::StartOfFrame()
{
	if(startOfFrameTime!=0)
		ticksSinceLastFrame = SDL_GetTicks() - startOfFrameTime;
	startOfFrameTime = SDL_GetTicks();
}

void FrameRateControler::EndOfFrame()
{
	currentTime = SDL_GetTicks();
	if (currentTime - startOfFrameTime < frameTimeTicks)
		SDL_Delay(frameTimeTicks - (currentTime - startOfFrameTime));
	if(SDL_GetTicks() - startOfFrameTime !=0)
		std::cout << 1000.0f / (SDL_GetTicks() - startOfFrameTime) << " FPS" << std::endl;
}

float FrameRateControler::DeltaTime()
{
	return (float)ticksSinceLastFrame / 1000.0f;
}

FrameRateControler::FrameRateControler()
{
}
