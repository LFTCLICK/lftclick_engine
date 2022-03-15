#include "pch.h"
#include "Door.h"
#include "FrameRateController.h"
#include "Enemy.h"

void Door::Start()
{
	trans = parent->getComponent<Transform>();
	sqCollider = parent->getComponent<SquareCollider>();
	draw = parent->getComponent<Drawable>();
	p = g_GameObjManager->FindObjectOfTag("player")->getComponent<Player>();
	zeroIndexDoorPhases = doorPhases-1;
	currentPhase = zeroIndexDoorPhases;
	hp = maxHp;
	hpPerPhase = maxHp / zeroIndexDoorPhases;
	g_EventManager->Subscribe(Message::TRIGGER_COLLISION, parent);
	g_EventManager->Subscribe(Message::COLLISION, parent);
	UpdateImage();
}

void Door::Update()
{
	if (playerInRange && currentPhase<zeroIndexDoorPhases)
	{
		//imgui stuff
		if (g_InputManager->isKeyPressed(SDL_SCANCODE_E) && p->wood> woodRequiredPerPhase)
		{
			ImGui::Text("Repairing door...");
			internalTimer += g_FrameRateController->DeltaTime();
		}
		else if (g_InputManager->isKeyReleased(SDL_SCANCODE_E))
		{
			internalTimer = 0;
		}
		if (internalTimer >= repairTime)
		{
			p->wood -= woodRequiredPerPhase;
			internalTimer = 0;
			currentPhase++;
			hp += hpPerPhase;
			UpdateImage();
			if (sqCollider->isTrigger)
				sqCollider->isTrigger = false;
		}
	}
	else
		internalTimer = 0;
	playerInRange = false;
	if (hp <= 0 && !sqCollider->isTrigger)
	{
		UpdateImage();
		sqCollider->isTrigger = true;
	}
}

void Door::Deserialize(nlohmann::json j, GameObject* parent)
{
	this->parent = parent;
	doorPhases = j["doorPhases"];
	maxHp = j["hp"];
	woodRequiredPerPhase = j["woodRequiredPerPhase"];
	repairTime = j["repairTime"];
}

Component* Door::Clone(GameObject* newParent)
{
	Door* toReturn = new Door();
	toReturn->doorPhases = doorPhases;
	toReturn->maxHp = maxHp;
	toReturn->woodRequiredPerPhase = woodRequiredPerPhase;
	toReturn->repairTime = repairTime;
	toReturn->parent = newParent;
	return toReturn;
}

void Door::UpdateImage()
{
	draw->xOffset = std::max(0.0f, ((float)currentPhase / doorPhases));
}

void Door::HandleMessage(Message* e)
{
	if (e->otherObject->parent->tag == "player")
	{
		playerInRange = true;
	}
	else if (e->otherObject->parent->tag == "enemy" && hp > 0)
	{
		Enemy* currentEnemy = e->otherObject->parent->getComponent<Enemy>();
		currentEnemy->timer+= g_FrameRateController->DeltaTime();
		if (currentEnemy->timer > currentEnemy->attackTimer)
		{
			currentEnemy->timer = 0;
			hp -= currentEnemy->damage;
			while(hp < maxHp * ((float)(currentPhase - 1) / zeroIndexDoorPhases))
			{
				currentPhase--;
				UpdateImage();
				if (currentPhase < 1)
				{
					sqCollider->isTrigger = true;
				}
			}
			
		}
	}
}
