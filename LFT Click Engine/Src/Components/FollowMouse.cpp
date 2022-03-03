// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	Gun.cpp
// Author			:	Chris Fitch
// Creation Date	:	2022/02/24
// Purpose			:	Contains functions for firing bullets
// History			: 
// ---------------------------------------------------------------------------
#include "pch.h"
#include "Gun.h"
#include "FollowMouse.h"
#include "GameManager.h"
#include "FrameRateController.h"
using json = nlohmann::json;

void FollowMouse::Start()
{
	trans = parent->getComponent<Transform>();
}

void FollowMouse::Update()
{
	while (ShowCursor(false) >= 0); // Hides cursor

	float targetX = (float)(InputManager::getInstance().mouseX() - 400) + GameManager::getInstance().mainCamera->xPos;
	float targetY = -1 * (float)(InputManager::getInstance().mouseY() - 400) + GameManager::getInstance().mainCamera->yPos;
	trans->SetPos(targetX, targetY);
}

void FollowMouse::Deserialize(nlohmann::json j, GameObject* parent)
{
	this->parent = parent;
}

Component* FollowMouse::Clone(GameObject* newParent)
{
	FollowMouse* toReturn = new FollowMouse();
	toReturn->parent = newParent;
	toReturn->trans = trans;
	toReturn->gom = gom;
	return toReturn;
}

FollowMouse::~FollowMouse()
{

}

void FollowMouse::HandleMessage(Message* e) {

}
