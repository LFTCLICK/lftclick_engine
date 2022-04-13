#include "pch.h"
#include "WoodPile.h"
#include "FrameRateController.h"
#include "Collider.h"

void WoodPile::Start()
{
	player = g_GameObjManager->FindObjectOfTag("player")->getComponent<Player>();
	myTransform = componentOwner->getComponent<Transform>();
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
			player->collectibleWood += woodPerCollect;
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

