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

//This has to be called if there is a considerable gap (especially because of loading) between end and start of frame
//ticksSinceLastFrame is used for DeltaTime and if there is a big gap due to loading the start of the game thinks the player needs to move a lot farther than they should

void FrameRateControler::Init(Uint32 ticksPerFrame)
{
	frameTimeTicks = ticksPerFrame;
	startOfFrameTime = 0;
	ticksSinceLastFrame = 0;
}

//sets the start of the frame time and the ammount of time since the last start was called
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
