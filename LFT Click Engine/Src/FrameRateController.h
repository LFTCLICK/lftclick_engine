// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	FrameRateController.h
// Author			:	Vance Howald
// Creation Date	:	2021/10/06
// Purpose			:	Frame Controler
// History			: 
// - 2021/10/29		Singleton
// ---------------------------------------------------------------------------
#pragma once
#include <SDL.h>
#include <sol/sol.hpp>

class FrameRateController
{
public:
	static FrameRateController& getInstance()
	{
		static FrameRateController instance;
		return instance;
	}
	FrameRateController(FrameRateController const&) = delete;
	void operator=(FrameRateController const&) = delete;
	void Init(int desiredFPS);

	void Tick();
	void Reset();
	void StartOfFrame();
	void EndOfFrame();
	float DeltaTime();

	void RegGlobals(sol::state& state);
private:
	FrameRateController();
	
	__int64 m_CurrentTime;
	__int64 m_PreviousTime;
	__int64 countsPerFrame;
	__int64 milisecConverter;
	double m_DeltaTime;
	double m_SecondsPerCount;
};