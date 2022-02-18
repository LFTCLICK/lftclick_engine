// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	FrameRateControler.h
// Author			:	Vance Howald
// Creation Date	:	2021/10/06
// Purpose			:	Frame Controler
// History			: 
// - 2021/10/29		Singleton
// ---------------------------------------------------------------------------
#pragma once
#include <SDL.h>

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
	void Init(Uint32 ticksPerFrame);

	void Tick();
	void Reset();
	float DeltaTime();
private:
	FrameRateController();
	
	__int64 m_CurrentTime;
	__int64 m_PreviousTime;

	double m_DeltaTime;
	double m_SecondsPerCount;
};