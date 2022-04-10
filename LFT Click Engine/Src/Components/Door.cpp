#include "pch.h"
#include "Door.h"
#include "FrameRateController.h"
#include "Enemy.h"

void Door::Start()
{
	myTransform = componentOwner->getComponent<Transform>();
	squareCollider = componentOwner->getComponent<SquareCollider>();
	drawable = componentOwner->getComponent<Drawable>();
	player = g_GameObjManager->FindObjectOfTag("player")->getComponent<Player>();
	zeroIndexDoorPhases = doorPhases-1;
	currentPhase = zeroIndexDoorPhases;
	health = maxHp;
	hpPerPhase = maxHp / zeroIndexDoorPhases;
	g_EventManager->Subscribe(Message::TRIGGER_COLLISION, componentOwner);
	g_EventManager->Subscribe(Message::COLLISION, componentOwner);
	UpdateImage();
}

void Door::Update()
{
	
	if (playerInRange)
	{
		if (!repairing && health < maxHp)
		{
			drawable->HUD_DrawTextCenter("Press E to repair the door", { 0.0f, -70.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });
		}
		if (currentPhase < zeroIndexDoorPhases)
		{
			if (g_InputManager->isKeyPressed(SDL_SCANCODE_E))
			{
				if (player->wood > woodRequiredPerPhase)
				{
					if (health < maxHp)
					{
						std::string text = "Repairing Door : " + std::to_string(static_cast<int>(internalTimer / repairTime * 100)) + "%";
						drawable->HUD_DrawTextCenter(text, { 0.0f, -70.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });

						repairing = true;
					}
					else
					{
						repairing = false;
					}

					internalTimer += g_FrameRateController->DeltaTime();
				}
				else
				{
					drawable->HUD_DrawTextCenter("Not enough wood to repair!", { 0.0f, -70.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });
					repairing = true;
				}
			}
			else /*if (g_InputManager->isKeyReleased(SDL_SCANCODE_E))*/
			{
				internalTimer = 0;
				repairing = false;
			}
			if (internalTimer >= repairTime)
			{
				player->wood -= woodRequiredPerPhase;
				internalTimer = 0;
				health += hpPerPhase;
				currentPhase = health / hpPerPhase;
				UpdateImage();
				if (squareCollider->isTrigger)
					squareCollider->isTrigger = false;
			}
		}
	}
	else
	{
		internalTimer = 0;
		repairing = false;
	}

	playerInRange = false;
	if (health <= 0 && !squareCollider->isTrigger)
	{
		UpdateImage();
		squareCollider->isTrigger = true;
	}
}

void Door::Deserialize(nlohmann::json j, GameObject* componentOwner)
{
	this->componentOwner = componentOwner;
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
	toReturn->componentOwner = newParent;
	return toReturn;
}

void Door::UpdateImage()
{
	drawable->xOffset = std::max(0.0f, ((float)currentPhase / doorPhases));
}

void Door::HandleMessage(Message* e)
{
	if (e->otherObject->componentOwner->tag == "player")
	{
		playerInRange = true;
	}
	else if (e->otherObject->componentOwner->tag == "zombie" && health > 0)
	{
		Enemy* currentEnemy = e->otherObject->componentOwner->getComponent<Enemy>();
		currentEnemy->timer+= g_FrameRateController->DeltaTime();
		if (currentEnemy->timer > currentEnemy->attackTimer)
		{
			currentEnemy->timer = 0;
			health -= currentEnemy->damage;
			if (health <= 0)
			{
				health = 0;
				currentPhase = 0;
			}
			else
			{
				currentPhase = health / hpPerPhase;
				currentPhase++;
			}
			UpdateImage();
			if (currentPhase < 1)
			{
				squareCollider->isTrigger = true;
			}
			
		}
	}
}
