#include "pch.h"
#include "Door.h"
#include "FrameRateController.h"
#include "Enemy.h"

void Door::Start()
{
	trans = parent->getComponent<Transform>();
	sqCollider = parent->getComponent<SquareCollider>();
	draw = parent->getComponent<Drawable>();
	p = GameObjectManager::getInstance().FindObjectOfTag("player")->getComponent<Player>();
	zeroIndexDoorPhases = doorPhases-1;
	currentPhase = zeroIndexDoorPhases;
	hp = maxHp;
	hpPerPhase = maxHp / zeroIndexDoorPhases;
	EventManager::getInstance().Subscribe(Message::TRIGGER_COLLISION, parent);
	EventManager::getInstance().Subscribe(Message::COLLISION, parent);
	sqCollider->isTrigger = true;
	UpdateImage();
}

void Door::Update()
{
	if (playerInRange && currentPhase<zeroIndexDoorPhases)
	{
		//imgui stuff
		if (InputManager::getInstance().isKeyPressed(SDL_SCANCODE_E) && p->wood> woodRequiredPerPhase)
		{
			ImGui::Text("Repairing door...");
			internalTimer += FrameRateController::getInstance().DeltaTime();
		}
		else if (InputManager::getInstance().isKeyReleased(SDL_SCANCODE_E))
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
	if (e->sourceObjectTag == "player")
	{
		playerInRange = true;
	}
	else if (e->sourceObjectTag == "enemy" && hp>0)
	{
		//Enemy* currentEnemy = e->otherObject->parent->getComponent<Enemy>();
		//currentEnemy->timer+= FrameRateController::getInstance().DeltaTime();
		//if (currentEnemy->timer > currentEnemy->attackTimer)
		//{
		//	currentEnemy->timer = 0;
		//	hp -= currentEnemy->damage;
		//	while(hp < maxHp * ((float)(currentPhase - 1) / zeroIndexDoorPhases))
		//	{
		//		currentPhase--;
		//		UpdateImage();
		//		if (currentPhase < 1)
		//		{
		//			sqCollider->isTrigger = true;
		//		}
		//	}
		//	
		//}
	}
}
