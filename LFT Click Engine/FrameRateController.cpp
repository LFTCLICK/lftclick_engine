// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	FrameRateControler.cpp
// Author			:	Vance Howald
// Creation Date	:	2021/10/06
// Purpose			:	Implementation
// History			:
// ---------------------------------------------------------------------------
#include "FrameRateController.h"
#include <iostream>
#include <Windows.h>
//This has to be called if there is a considerable gap (especially because of loading) between end and start of frame
//ticksSinceLastFrame is used for DeltaTime and if there is a big gap due to loading the start of the game thinks the player needs to move a lot farther than they should

void FrameRateController::Init(Uint32 ticksPerFrame)
{
	Reset();
}

FrameRateController::FrameRateController() :
	m_CurrentTime(0), m_PreviousTime(0),
	m_DeltaTime(0)
{
	__int64 countsPerSecond;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSecond);

	m_SecondsPerCount = 1.0 / (double)countsPerSecond;

	Reset();
}

void FrameRateController::Reset()
{
	__int64 currentTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

	m_PreviousTime = currentTime;
}



////sets the start of the frame time and the ammount of time since the last start was called
//void FrameRateControler::StartOfFrame()
//{
//	if(startOfFrameTime!=0)
//		ticksSinceLastFrame = SDL_GetTicks() - startOfFrameTime;
//	startOfFrameTime = SDL_GetTicks();
//}
//
//void FrameRateControler::EndOfFrame()
//{
//	currentTime = SDL_GetTicks();
//	if (currentTime - startOfFrameTime < frameTimeTicks)
//		SDL_Delay(frameTimeTicks - (currentTime - startOfFrameTime));
//	if(SDL_GetTicks() - startOfFrameTime !=0)
//		std::cout << 1000.0f / (SDL_GetTicks() - startOfFrameTime) << " FPS" << std::endl;
//}

void FrameRateController::Tick()
{
	__int64 currentTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

	m_CurrentTime = currentTime;
	m_DeltaTime = (m_CurrentTime - m_PreviousTime) * m_SecondsPerCount;
	m_PreviousTime = m_CurrentTime;

	if (m_DeltaTime < 0.0)
		m_DeltaTime = 0.0;

	std::cout << m_DeltaTime << " Deltatime (Seconds)" << std::endl;
}

float FrameRateController::DeltaTime()
{
	return static_cast<float>(m_DeltaTime);
}


