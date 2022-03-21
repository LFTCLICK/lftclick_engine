// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	Interactable.h
// Author			:	Chris Fitch
// Creation Date	:	2022/03/20
// Purpose			:	Makes object interactable
// History			: 
// ---------------------------------------------------------------------------

#include "pch.h"
#include "Interactable.h"
#include "FrameRateController.h"
#include "Collider.h"

void Interactable::Start()
{
	p = g_GameObjManager->FindObjectOfTag("player")->getComponent<Player>();
	trans = componentOwner->getComponent<Transform>();
}

void Interactable::Update()
{
	if (playerIsInRange)
	{
		//imgui stuff
		if (g_InputManager->isKeyPressed(SDL_SCANCODE_E))
		{
			ImGui::Text("Interacting...");
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
}

Component* Interactable::Clone(GameObject* newParent)
{
	Interactable* toReturn = new Interactable();
	toReturn->componentOwner = newParent;
	toReturn->woodPerCollect = woodPerCollect;
	toReturn->timeToCollect = timeToCollect;
	return (Component*)toReturn;
}

void Interactable::Deserialize(nlohmann::json j, GameObject* componentOwner)
{
	this->componentOwner = componentOwner;
	timeToCollect = j["timeToCollect"];
	woodPerCollect = j["woodPerCollect"];
}


void Interactable::HandleMessage(Message* e)
{
	
}