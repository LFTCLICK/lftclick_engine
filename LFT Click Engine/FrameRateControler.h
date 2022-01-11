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
	void StartOfFrame();
	void EndOfFrame();
	float DeltaTime();
private:
	FrameRateControler();
	Uint32 startOfFrameTime;
	Uint32 currentTime;
	Uint32 frameTimeTicks;
	Uint32 ticksSinceLastFrame;
};