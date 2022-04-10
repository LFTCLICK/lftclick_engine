// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	Interactable.cpp
// Author			:	Chris Fitch
// Creation Date	:	2022/03/20
// Purpose			:	Makes object interactable
// History			: 
// ---------------------------------------------------------------------------

#include "pch.h"
#include "Interactable.h"

const Audible::SoundEvent AUDIO_ON_INTERACTING = Audible::SoundEvent::AUDIO_ON_INTERACTING;
const Audible::SoundEvent AUDIO_ON_COLLECT = Audible::SoundEvent::AUDIO_ON_COLLECT;

bool Interactable::tutorialUI = true;

void Interactable::Start()
{
	myTransform = componentOwner->getComponent<Transform>();
	audio = componentOwner->getComponent<Audible>();
	anim = componentOwner->getComponent<SpriteAnimator>();
	drawable = componentOwner->getComponent<Drawable>();

	interactDistanceSq = interactDistance * interactDistance;

	currentHp = hpPerPhase;
}

void Interactable::Update()
{
	playerIsInRange = IsPlayerInRange();

	if (playerIsInRange) {
		if (g_InputManager->isKeyTriggered(SDL_SCANCODE_E))
			StartInteraction();
		else if (g_InputManager->isKeyReleased(SDL_SCANCODE_E))
			StopInteraction();

		if (tutorialUI)
			drawable->HUD_DrawTextCenter("Hold E to Destroy", { 0.0f, -100.0f}, { 1.0f, 1.0f, 1.0f, 1.0f });

		if (interacting) 
		{
#ifdef _DEBUG
			ImGui::Text("Interacting...");
#endif		
			
			int destroyedProgress = static_cast<int>(internalTimer / timeToCollect * 100);
			std::string progressText = "Collecting: " + std::to_string(destroyedProgress) + "%";

			drawable->HUD_DrawTextCenter(progressText, { 0.0f, -100.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });

			internalTimer += g_FrameRateController->DeltaTime();
		}

		if (internalTimer >= timeToCollect)
			CompleteInteraction();

		if (!playerWasInRange)
			anim->InRange();
	}
	else if (playerWasInRange) {
		anim->OutOfRange();
		StopInteraction();
	}

	playerWasInRange = playerIsInRange;
}

void Interactable::StartInteraction() {
	audio->PlaySoundsOnEvent(AUDIO_ON_INTERACTING);
	interacting = true;
	if (tutorialUI) tutorialUI = false;
}

void Interactable::ContinueInteraction() {
	audio->PlaySoundsOnEvent(AUDIO_ON_INTERACTING);
	interacting = true;
}

void Interactable::StopInteraction() {
	internalTimer = 0;
	if (interacting) {
		interacting = false;
		audio->StopSoundsOnEvent(AUDIO_ON_INTERACTING);
	}
}

void Interactable::CompleteInteraction() {
	audio->PlaySoundsOnEvent(AUDIO_ON_COLLECT);

	if (hasParts) {
		if (((float)rand() / RAND_MAX) < g_GameManager->GetChanceOfFindingPart()) {
			g_GameManager->playerObj->getComponent<Player>()->parts++;
			g_GameManager->PartSearchSuccessful();
		}
		else {
			g_GameManager->playerObj->getComponent<Player>()->wood += woodPerCollect;
			g_GameManager->PartSearchFailed();
		}
	}
	else {
		g_GameManager->playerObj->getComponent<Player>()->wood += woodPerCollect;
	}

	internalTimer = 0;
	if (--currentHp < 1) {
		++currentPhase;
		if (currentPhase < totalPhases) {
			currentHp = hpPerPhase;
			anim->SwitchPhase(currentPhase);
		}
		else
			componentOwner->isDeletable = true;
	}
}

bool Interactable::IsPlayerInRange() {
	DirectX::SimpleMath::Vector2 pos = myTransform->CurrentPos(), playerPos = g_GameManager->playerTrans->CurrentPos();

	return (pos - playerPos).LengthSquared() < interactDistanceSq;
}

Component* Interactable::Clone(GameObject* newParent)
{
	Interactable* toReturn = new Interactable();
	toReturn->componentOwner = newParent;
	toReturn->woodPerCollect = woodPerCollect;
	toReturn->timeToCollect = timeToCollect;
	toReturn->interactDistance = interactDistance;
	toReturn->totalPhases = totalPhases;
	toReturn->hpPerPhase = hpPerPhase;
	toReturn->hasParts = hasParts;
	return (Component*)toReturn;
}

void Interactable::Deserialize(nlohmann::json j, GameObject* componentOwner)
{
	this->componentOwner = componentOwner;
	if (j.contains("timeToCollect")) timeToCollect = j["timeToCollect"];
	if (j.contains("woodPerCollect")) woodPerCollect = j["woodPerCollect"];
	if (j.contains("interactDistance")) interactDistance = j["interactDistance"];
	if (j.contains("totalPhases")) totalPhases = j["totalPhases"];
	if (j.contains("hpPerPhase")) hpPerPhase = j["hpPerPhase"];
	if (j.contains("hasParts")) hpPerPhase = j["hasParts"];
}


void Interactable::HandleMessage(Message* e)
{
	
}