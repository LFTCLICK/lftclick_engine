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
#include "FrameRateController.h"
using json = nlohmann::json;

void FollowMouse::Start()
{
	cam = gom->FindObjectOfTag("player")->getComponent<Camera>();
	trans = parent->getComponent<Transform>();
}

void FollowMouse::Update()
{
	float targetX = (float)(InputManager::getInstance().mouseX() - 400) + cam->xPos;
	float targetY = -1 * (float)(InputManager::getInstance().mouseY() - 400) + cam->yPos;
	trans->SetPos(targetX, targetY);
}

Component* FollowMouse::Clone(GameObject* newParent)
{
	FollowMouse* toReturn = new FollowMouse();
	toReturn->trans = trans;
	toReturn->cam = cam;
	toReturn->gom = gom;
	return toReturn;
}

FollowMouse::FollowMouse(json j, GameObject* parent) : parent(parent)
{
	gom = &GameObjectManager::getInstance();
}

FollowMouse::~FollowMouse()
{

}
