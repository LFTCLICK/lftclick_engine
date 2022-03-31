// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	FrameRateController.h
// Author			:	Abhijit Zala
// Creation Date	:	2021/10/06
// Purpose			:	Frame Controller
// ---------------------------------------------------------------------------
#pragma once
#include <SDL.h>
#include <sol/sol.hpp>

class FrameRateController
{
public:
	FrameRateController();
	FrameRateController(FrameRateController const&) = delete;
	void operator=(FrameRateController const&) = delete;
	void Init(int desiredFPS);

	void Tick();
	void Reset();
	float DeltaTime();

private:
	__int64 m_CurrentTime;
	__int64 m_PreviousTime;
	__int64 countsPerFrame;
	__int64 milisecConverter;
	double m_DeltaTime;
	double m_SecondsPerCount;
};

extern std::unique_ptr<FrameRateController> g_FrameRateController;