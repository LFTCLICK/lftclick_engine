// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	WoodPile.cpp
// Author			:	Vance Howald
// Creation Date	:	2021/03/09
// Purpose			:	Wood collection zones for repairing the doors
// History			:
// ---------------------------------------------------------------------------
#include "pch.h"
#include "WoodPile.h"
#include "FrameRateController.h"
#include "Collider.h"

void WoodPile::Start()
{
	p = g_GameObjManager->FindObjectOfTag("player")->getComponent<Player>();
	trans = componentOwner->getComponent<Transform>();
}

void WoodPile::Update()
{
	if (playerInRange)
	{
		//imgui stuff
		if (g_InputManager->isKeyPressed(SDL_SCANCODE_E))
		{
			ImGui::Text("Collecting wood...");
			internalTimer += g_FrameRateController->DeltaTime();
		}
		else if (g_InputManager->isKeyReleased(SDL_SCANCODE_E))
		{
			internalTimer = 0;
		}
		if (internalTimer >= timeToCollect)
		{
			p->wood += woodPerCollect;
			internalTimer = 0;
			currentPhase++;
		}
	}
	else
		internalTimer = 0;
	playerInRange = false;
}

Component* WoodPile::Clone(GameObject* newParent)
{
	WoodPile* toReturn = new WoodPile();
	toReturn->componentOwner = newParent;
	toReturn->woodPerCollect = woodPerCollect;
	toReturn->timeToCollect = timeToCollect;
	return (Component*)toReturn;
}

void WoodPile::Deserialize(nlohmann::json j, GameObject* componentOwner)
{
	this->componentOwner = componentOwner;
	timeToCollect = j["timeToCollect"];
	woodPerCollect = j["woodPerCollect"];
}


void WoodPile::HandleMessage(Message* e)
{
	if (e->otherObject->componentOwner->tag == "player")
	{
		playerInRange = true;
	}
}

