// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	FollowMouse.cpp
// Author			:	Chris Fitch
// Creation Date	:	2022/02/24
// Purpose			:	Camera mouse following script
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
	myTransform = componentOwner->getComponent<Transform>();

}

void FollowMouse::Update()
{
	if (!g_FrameRateController->zeroDeltaTime) {

		while (ShowCursor(false) >= 0); // Hides cursor

		if (g_GameManager->currentLevel == EGameLevel::Pausemenu)
			return;

		float targetX = (float)(g_InputManager->mouseX() - g_Renderer->GetWidth()/2.0f) + g_GameManager->mainCamera->xPos;
		float targetY = -1 * (float)(g_InputManager->mouseY() - g_Renderer->GetHeight() / 2.0f) + g_GameManager->mainCamera->yPos;

		myTransform->SetPos(targetX, targetY);
	}
}

void FollowMouse::Deserialize(nlohmann::json j, GameObject* componentOwner)
{
	this->componentOwner = componentOwner;
}

Component* FollowMouse::Clone(GameObject* newParent)
{
	FollowMouse* toReturn = new FollowMouse();
	toReturn->componentOwner = newParent;
	toReturn->myTransform = myTransform;
	toReturn->gom = gom;
	return toReturn;
}

FollowMouse::~FollowMouse()
{
	while (ShowCursor(true) < 0); // Shows cursor
}