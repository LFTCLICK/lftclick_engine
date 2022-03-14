#include "pch.h"
#include "WoodPile.h"
#include "FrameRateController.h"
#include "Collider.h"

void WoodPile::Start()
{
	p = GameObjectManager::getInstance().FindObjectOfTag("player")->getComponent<Player>();
	trans = parent->getComponent<Transform>();
}

void WoodPile::Update()
{
	if (playerInRange)
	{
		//imgui stuff
		if (InputManager::getInstance().isKeyPressed(SDL_SCANCODE_E))
		{
			ImGui::Text("Collecting wood...");
			internalTimer += FrameRateController::getInstance().DeltaTime();
		}
		else if (InputManager::getInstance().isKeyReleased(SDL_SCANCODE_E))
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
	toReturn->parent = newParent;
	toReturn->woodPerCollect = woodPerCollect;
	toReturn->timeToCollect = timeToCollect;
	return (Component*)toReturn;
}

void WoodPile::Deserialize(nlohmann::json j, GameObject* parent)
{
	this->parent = parent;
	timeToCollect = j["timeToCollect"];
	woodPerCollect = j["woodPerCollect"];
}


void WoodPile::HandleMessage(Message* e)
{
	if (e->otherObject->parent->tag == "player")
	{
		playerInRange = true;
	}
}

