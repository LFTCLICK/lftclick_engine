#include "pch.h"
#include "Door.h"
#include "FrameRateController.h"
#include "CircleCollider.h"
#include "Enemy.h"

const Audible::SoundEvent AUDIO_ON_DOOR_BASH = Audible::SoundEvent::AUDIO_ON_DOOR_BASH;

void Door::Start()
{
	myTransform = componentOwner->getComponent<Transform>();
	squareCollider = componentOwner->getComponent<SquareCollider>();
	drawable = componentOwner->getComponent<Drawable>();
	player = g_GameObjManager->FindObjectOfTag("player")->getComponent<Player>();
	zeroIndexDoorPhases = doorPhases-1;
	currentPhase = zeroIndexDoorPhases;
	playerHealth = maxHp;
	hpPerPhase = maxHp / zeroIndexDoorPhases;
	g_EventManager->Subscribe(Message::TRIGGER_COLLISION, componentOwner);
	g_EventManager->Subscribe(Message::COLLISION, componentOwner);
	UpdateImage();
	inWoodPilePhase = false;
}

void Door::Update()
{
	
	if (playerInRange)
	{
		if (inWoodPilePhase)
		{
			drawable->HUD_DrawTextCenter("Press Q to replace the door", { 0.0f, -70.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });
			if (g_InputManager->isKeyTriggered(SDL_SCANCODE_Q))
			{
				inWoodPilePhase = false;
				float backupX;
				backupX = squareCollider->clientWidth;
				squareCollider->clientWidth = woodPileRadius;
				componentOwner->getComponent<CircleCollider>()->radius = woodPileRadius/2;
				woodPileRadius = backupX/2;
				myTransform->position -= woodPileOffset;
				drawable->yOffset = 0;
				UpdateImage();
				squareCollider->isTrigger = false;
			}

		}
		else
		{
			bool hasText = false;
			if (g_InputManager->isKeyTriggered(SDL_SCANCODE_Q))
			{
				inWoodPilePhase = true;
				float backupX;
				backupX = squareCollider->clientWidth;
				squareCollider->clientWidth = woodPileRadius * 2;
				componentOwner->getComponent<CircleCollider>()->radius = woodPileRadius;
				woodPileRadius = backupX;
				myTransform->position += woodPileOffset;
				drawable->yOffset = .5;
				drawable->xOffset = 0;
				squareCollider->isTrigger = true;
			}
			if (!repairing && playerHealth < maxHp)
			{
				drawable->HUD_DrawTextCenter("Hold E to repair the door\nPress Q to remove", { 0.0f, -70.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });
				hasText = true;
			}
			if (currentPhase < zeroIndexDoorPhases)
			{
				if (g_InputManager->isKeyPressed(SDL_SCANCODE_E))
				{
					if (player->collectibleWood > woodRequiredPerPhase)
					{
						if (playerHealth < maxHp)
						{
							std::string text = "Repairing Door : " + std::to_string(static_cast<int>(internalTimer / repairTime * 100)) + "%";
							drawable->HUD_DrawTextCenter(text, { 0.0f, -70.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });
							hasText = true;

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
						hasText = true;
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
					player->collectibleWood -= woodRequiredPerPhase;
					player->playCollectedAnimWood = true;

					internalTimer = 0;
					playerHealth += hpPerPhase;
					currentPhase = playerHealth / hpPerPhase;
					UpdateImage();
					if (squareCollider->isTrigger)
						squareCollider->isTrigger = false;
				}
				if(!hasText)
					drawable->HUD_DrawTextCenter("Press Q to remove the door", { 0.0f, -70.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });
			}
		}
	}
	else
	{
		internalTimer = 0;
		repairing = false;
	}

	playerInRange = false;
	if (playerHealth <= 0 && !squareCollider->isTrigger)
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
	std::vector<float> woodPileOffsetHelper = j["woodPileOffset"].get<std::vector<float>>();
	woodPileOffset = { woodPileOffsetHelper[0], woodPileOffsetHelper[1] };
	woodPileRadius = j["woodPileRadius"];
}

Component* Door::Clone(GameObject* newParent)
{
	Door* toReturn = new Door();
	toReturn->doorPhases = doorPhases;
	toReturn->maxHp = maxHp;
	toReturn->woodRequiredPerPhase = woodRequiredPerPhase;
	toReturn->woodPileOffset = woodPileOffset;
	toReturn->woodPileRadius = woodPileRadius;
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
	else if (e->otherObject->componentOwner->tag == "zombie" && playerHealth > 0 && !inWoodPilePhase)
	{
		Enemy* currentEnemy = e->otherObject->componentOwner->getComponent<Enemy>();
		currentEnemy->timer+= g_FrameRateController->DeltaTime();
		if (currentEnemy->timer > currentEnemy->attackTimer)
		{
			e->otherObject->componentOwner->getComponent<Audible>()->PlaySoundsOnEvent(AUDIO_ON_DOOR_BASH);
			currentEnemy->timer = 0;
			playerHealth -= currentEnemy->damage;
			if (playerHealth <= 0)
			{
				playerHealth = 0;
				currentPhase = 0;
			}
			else
			{
				currentPhase = playerHealth / hpPerPhase;
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
