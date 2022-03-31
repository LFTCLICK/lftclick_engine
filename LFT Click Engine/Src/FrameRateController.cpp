// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	FrameRateController.cpp
// Author			:	Abhijit Zala
// Creation Date	:	2021/10/06
// Purpose			:	Frame Controller
// ---------------------------------------------------------------------------
#include "pch.h"
#include "FrameRateController.h"
#include <iostream>
#include <Windows.h>
#include <synchapi.h>


void FrameRateController::Init(int desiredFPS)
{
	__int64 countsPerSecond;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSecond);
	countsPerFrame = countsPerSecond / desiredFPS;
	milisecConverter = countsPerSecond / 1000;
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

void FrameRateController::Tick()
{
	__int64 endTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&endTime);
	if (endTime - m_CurrentTime < countsPerFrame)
	{
		__int64 sleepStartTime = endTime;
		__int64 timeToWait = countsPerFrame - (endTime - m_CurrentTime);
		DWORD msSecs = timeToWait / milisecConverter;
		if (msSecs > 2)
		{
			Sleep(msSecs-1);
		}
		do
		{
			QueryPerformanceCounter((LARGE_INTEGER*)&endTime);

		} while (endTime - sleepStartTime < timeToWait);
	}
	m_CurrentTime = endTime;
	m_DeltaTime = (m_CurrentTime - m_PreviousTime) * m_SecondsPerCount;
	m_PreviousTime = m_CurrentTime;

	if (m_DeltaTime < 0.0)
		m_DeltaTime = 0.0;
	if (g_GameManager->playerDead)
		m_DeltaTime = 0;
}

float FrameRateController::DeltaTime()
{
	return static_cast<float>(m_DeltaTime);
}


