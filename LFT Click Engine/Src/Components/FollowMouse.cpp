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
#include "Renderer.h"

using json = nlohmann::json;

using namespace DirectX::SimpleMath;

void FollowMouse::Start()
{
	trans = parent->getComponent<Transform>();

}

void FollowMouse::Update()
{
#ifndef _DEBUG
	while (ShowCursor(false) >= 0); // Hides cursor
#endif

	float targetX = (float)(g_InputManager->mouseX() - g_Renderer->GetWidth()/2.0f) + g_GameManager->mainCamera->xPos;
	float targetY = -1 * (float)(g_InputManager->mouseY() - g_Renderer->GetHeight() / 2.0f) + g_GameManager->mainCamera->yPos;

#ifdef _DEBUG
	ImGui::Text("Mouse: %d %d\n", g_InputManager->mouseX(), g_InputManager->mouseY());
#endif

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