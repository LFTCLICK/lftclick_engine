// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	InputManager.h
// Author			:	Vance Howald
// Creation Date	:	2021/10/06
// Purpose			:	Manages input and lets the engine read keypresses, holds, and releases
// History			: 
// - 2021/10/29		Singleton
// ---------------------------------------------------------------------------
#pragma once
#include <SDL_stdinc.h>

class Input_Manager
{
public:
	static Input_Manager& getInstance()
	{
		static Input_Manager instance;
		return instance;
	}
	Input_Manager(Input_Manager const&) = delete;
	void operator=(Input_Manager const&) = delete;
	~Input_Manager();
	void Update();
	bool isKeyPressed(unsigned int KeyScanValue);
	bool isKeyReleased(unsigned int keyScanValue);
	bool isKeyTriggered(unsigned int keyScanValue);
	int mouseX();
	int mouseY();
	bool isMouseButtonPressed(unsigned int buttonIndex);
	bool isMouseButtonReleased(unsigned int buttonIndex);
	bool isMouseButtonTriggered(unsigned int buttonIndex);
	bool isJoyStickMovedUp(unsigned int joystickValue);
	bool isJoyStickMovedDown(unsigned int joystickValue);
	bool isJoyStickMovedLeft(unsigned int joystickValue);
	bool isJoyStickMovedRight(unsigned int joystickValue);

	

private:
	Input_Manager();
	Uint8 mCurrentState[512];
	Uint8 mPreviousState[512];
	int mousePos[2];
	bool mouseButtonCurrentState[3];
	bool mouseButtonPreviousState[3];

	Sint16 mCurrentAxesState[6];
	Sint16 mPreviousAxesState[6];
};