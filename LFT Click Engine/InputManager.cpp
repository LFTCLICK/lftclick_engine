// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	InputManager.cpp
// Author			:	Vance Howald
// Creation Date	:	2021/10/06
// Purpose			:	Implementation
// History			:
// ---------------------------------------------------------------------------
#include "InputManager.h"
#include <memory.h>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>
#include <iostream>

Input_Manager::Input_Manager()
{
	memset(mCurrentState, 0, 512 * sizeof(Uint8));
	memset(mouseButtonCurrentState, 0, 3 * sizeof(bool));
}

Input_Manager::~Input_Manager()
{
}

void Input_Manager::Update()
{
	int numberOfItems = 0;
	const Uint8* currentKeyStates = SDL_GetKeyboardState(&numberOfItems);//getting keyboard
	if (numberOfItems > 512)
		numberOfItems = 512;
	memcpy(mPreviousState, mCurrentState, 512);
	memcpy(mCurrentState, currentKeyStates, numberOfItems * sizeof(Uint8));
	memcpy(mouseButtonPreviousState, mouseButtonCurrentState, 3);

	Uint32 buttons = SDL_GetMouseState(&mousePos[0], &mousePos[1]);//getting mouse pos

	mouseButtonCurrentState[0] = (buttons & SDL_BUTTON_LMASK) != 0;//get mouse buttons
	mouseButtonCurrentState[1] = (buttons & SDL_BUTTON_RMASK) != 0;
	mouseButtonCurrentState[2] = (buttons & SDL_BUTTON_MMASK) != 0;
}

bool Input_Manager::isKeyPressed(unsigned int keyScanValue)
{
	if (keyScanValue >= 512)
		return false;
	return mCurrentState[keyScanValue]!=0;
}

bool Input_Manager::isKeyReleased(unsigned int keyScanValue)
{
	if (keyScanValue >= 512)
		return false;
	return mPreviousState[keyScanValue] != 0 && mCurrentState[keyScanValue] == 0;
}

bool Input_Manager::isKeyTriggered(unsigned int keyScanValue)
{
	if (keyScanValue >= 512)
		return false;
	return mPreviousState[keyScanValue] == 0 && mCurrentState[keyScanValue] != 0;
}

int Input_Manager::mouseX()
{
	return mousePos[0];
}

int Input_Manager::mouseY()
{
	return mousePos[1];
}

bool Input_Manager::isMouseButtonPressed(unsigned int buttonIndex)
{
	if (buttonIndex >= 3)
		return false;
	return mouseButtonCurrentState[buttonIndex];
}

bool Input_Manager::isMouseButtonReleased(unsigned int buttonIndex)
{
	if (buttonIndex >= 3)
		return false;
	return  mouseButtonPreviousState[buttonIndex] && mouseButtonCurrentState[buttonIndex];
}

bool Input_Manager::isMouseButtonTriggered(unsigned int buttonIndex)
{
	if (buttonIndex >= 3)
		return false;
	return  mouseButtonPreviousState[buttonIndex] && !mouseButtonCurrentState[buttonIndex];
}
