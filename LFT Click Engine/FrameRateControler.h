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

class FrameRateControler
{
public:
	static FrameRateControler& getInstance()
	{
		static FrameRateControler instance;
		return instance;
	}
	FrameRateControler(FrameRateControler const&) = delete;
	void operator=(FrameRateControler const&) = delete;
	void Init(Uint32 ticksPerFrame);
	//void StartOfFrame();
	//void EndOfFrame();

	void Tick();
	void Reset();
	float DeltaTime();
private:
	FrameRateControler();
	
	__int64 m_CurrentTime;
	__int64 m_PreviousTime;

	double m_DeltaTime;
	double m_SecondsPerCount;
};