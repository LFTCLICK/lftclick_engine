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

void Interactable::Start()
{
	trans = componentOwner->getComponent<Transform>();
	audio = componentOwner->getComponent<Audible>();
	anim = componentOwner->getComponent<SpriteAnimator>();
	interactDistanceSq = interactDistance * interactDistance;
}

void Interactable::Update()
{
	playerIsInRange = IsPlayerInRange();

	if (playerIsInRange) {

		if (g_InputManager->isKeyTriggered(SDL_SCANCODE_E))
			StartInteraction();
		else if (g_InputManager->isKeyReleased(SDL_SCANCODE_E))
			StopInteraction();

		if (interacting) {
			ImGui::Text("Interacting...");
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

	if (((float)rand() / RAND_MAX) < g_GameManager->GetChanceOfFindingPart()) {
		g_GameManager->playerObj->getComponent<Player>()->parts++;
		g_GameManager->PartSearchSuccessful();
	}
	else {
		g_GameManager->playerObj->getComponent<Player>()->wood += woodPerCollect;
		g_GameManager->PartSearchFailed();
	}


	internalTimer = 0;
	if (--currentHp < 1) {
		if (++currentPhase < totalPhases) {
			currentHp = hpPerPhase;
			anim->SwitchPhase(currentPhase);
		}
		else
			componentOwner->isDeletable = true;
	}
}

bool Interactable::IsPlayerInRange() {
	DirectX::SimpleMath::Vector2 pos = trans->CurrentPos(), playerPos = g_GameManager->playerTrans->CurrentPos();
	float distanceX = abs(pos.x - playerPos.x), distanceY = abs(pos.y - playerPos.y);
	return (distanceX * distanceX + distanceY * distanceY) < interactDistanceSq;

}

Component* Interactable::Clone(GameObject* newParent)
{
	Interactable* toReturn = new Interactable();
	toReturn->componentOwner = newParent;
	toReturn->woodPerCollect = woodPerCollect;
	toReturn->timeToCollect = timeToCollect;
	toReturn->interactDistance = interactDistance;
	return (Component*)toReturn;
}

void Interactable::Deserialize(nlohmann::json j, GameObject* componentOwner)
{
	this->componentOwner = componentOwner;
	timeToCollect = j["timeToCollect"];
	woodPerCollect = j["woodPerCollect"];
	interactDistance = j["interactDistance"];
}


void Interactable::HandleMessage(Message* e)
{
	
}