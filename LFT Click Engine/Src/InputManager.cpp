// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	InputManager.cpp
// Author			:	Vance Howald
// Creation Date	:	2021/10/06
// Purpose			:	Implementation
// History			:
// ---------------------------------------------------------------------------
#include "pch.h"
#include "InputManager.h"


SDL_Joystick* gGameController = nullptr;

InputManager::InputManager()
{
	memset(mCurrentState, 0, 512 * sizeof(Uint8));
	memset(mouseButtonCurrentState, 0, 3 * sizeof(bool));

	memset(mCurrentAxesState, 0, 6 * sizeof(Sint16));

	gGameController = SDL_JoystickOpen(0);
}

InputManager::~InputManager()
{
	//Close game controller
	SDL_JoystickClose(gGameController);
	gGameController = NULL;
}

void InputManager::Update()
{
	int numberOfItems = 0;
	
	const Uint8* currentKeyStates = SDL_GetKeyboardState(&numberOfItems);//getting keyboard
	if (numberOfItems > 512)
		numberOfItems = 512;
	memcpy(mPreviousState, mCurrentState, 512);
	memcpy(mCurrentState, currentKeyStates, numberOfItems * sizeof(Uint8));
	

	memcpy(mPreviousAxesState, mCurrentAxesState, 6 * sizeof(Sint16));
	for (int i = 0; i < 6; i++)
	{
		mCurrentAxesState[i] = SDL_JoystickGetAxis(gGameController, i);
	}

	memcpy(mouseButtonPreviousState, mouseButtonCurrentState, 3);
	Uint32 buttons = SDL_GetMouseState(&mousePos[0], &mousePos[1]);//getting mouse pos

	mouseButtonCurrentState[0] = (buttons & SDL_BUTTON_LMASK) != 0;//get mouse buttons
	mouseButtonCurrentState[1] = (buttons & SDL_BUTTON_RMASK) != 0;
	mouseButtonCurrentState[2] = (buttons & SDL_BUTTON_MMASK) != 0;
}

bool InputManager::isKeyPressed(unsigned int keyScanValue)
{
	if (keyScanValue >= 512)
		return false;
	return mCurrentState[keyScanValue]!=0;
}

bool InputManager::isKeyReleased(unsigned int keyScanValue)
{
	if (keyScanValue >= 512)
		return false;
	return mPreviousState[keyScanValue] != 0 && mCurrentState[keyScanValue] == 0;
}

bool InputManager::isKeyTriggered(unsigned int keyScanValue)
{
	if (keyScanValue >= 512)
		return false;
	return mPreviousState[keyScanValue] == 0 && mCurrentState[keyScanValue] != 0;
}

int InputManager::mouseX()
{
	return mousePos[0];
}

int InputManager::mouseY()
{
	return mousePos[1];
}

bool InputManager::isMouseButtonPressed(unsigned int buttonIndex)
{
	if (buttonIndex >= 3)
		return false;
	return mouseButtonCurrentState[buttonIndex];
}

bool InputManager::isMouseButtonReleased(unsigned int buttonIndex)
{
	if (buttonIndex >= 3)
		return false;
	return  mouseButtonPreviousState[buttonIndex] && mouseButtonCurrentState[buttonIndex];
}

bool InputManager::isMouseButtonTriggered(unsigned int buttonIndex)
{
	if (buttonIndex >= 3)
		return false;
	return  mouseButtonPreviousState[buttonIndex] && !mouseButtonCurrentState[buttonIndex];
}

bool InputManager::isJoyStickMovedUp(unsigned int joystickValue)
{
	if (mCurrentAxesState[joystickValue] > -8000)
		return false;
	return true;
	//return mCurrentAxesState[0];
}

bool InputManager::isJoyStickMovedDown(unsigned int joystickValue)
{
	if (mCurrentAxesState[joystickValue] < 8000)
		return false;
	return true;
	//return mCurrentAxesState[0];
}

bool InputManager::isJoyStickMovedLeft(unsigned int joystickValue)
{
	if (mCurrentAxesState[joystickValue] > -8000)
		return false;
	return true;
	//return mCurrentAxesState[1];
}

bool InputManager::isJoyStickMovedRight(unsigned int joystickValue)
{
	if (mCurrentAxesState[joystickValue] < 8000)
		return false;
	return true;
	//return mCurrentAxesState[1];
}



