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

const Audible::SoundEvent AUDIO_ON_INTERACTING = Audible::SoundEvent::AUDIO_ON_INTERACTING;
const Audible::SoundEvent AUDIO_ON_COLLECT = Audible::SoundEvent::AUDIO_ON_COLLECT;

bool Interactable::tutorialUI = true;

void Interactable::Start()
{
	trans = componentOwner->getComponent<Transform>();
	audio = componentOwner->getComponent<Audible>();
	anim = componentOwner->getComponent<SpriteAnimator>();
	drawable = componentOwner->getComponent<Drawable>();

	interactDistanceSq = interactDistance * interactDistance;
}

void Interactable::Update()
{
	playerIsInRange = IsPlayerInRange();

	if (playerIsInRange) {

		if (tutorialUI)
			drawable->HUD_DrawTextCenter("Press E to intereact", { 0.0f, -100.0f});
		
		if (g_InputManager->isKeyTriggered(SDL_SCANCODE_E)) {
			audio->PlaySoundsOnEvent(AUDIO_ON_INTERACTING);
			interacting = true;
			tutorialUI = false;
		}
		else if (g_InputManager->isKeyReleased(SDL_SCANCODE_E)) {
			internalTimer = 0;
			interacting = false;
			audio->StopSoundsOnEvent(AUDIO_ON_INTERACTING);
		}

		if (interacting) {
			ImGui::Text("Interacting...");
			internalTimer += g_FrameRateController->DeltaTime();
		}

		if (internalTimer >= timeToCollect) {
			audio->PlaySoundsOnEvent(AUDIO_ON_COLLECT);
			g_GameManager->playerObj->getComponent<Player>()->wood += woodPerCollect;
			internalTimer = 0;
			currentPhase++;
		}
	}
	else {
		internalTimer = 0;
		if (interacting) {
			interacting = false;
			audio->StopSoundsOnEvent(AUDIO_ON_INTERACTING);
		}
	}

	if (playerIsInRange != playerWasInRange) {
		if (playerIsInRange) anim->InRange();
		else anim->OutOfRange();
	}

	playerWasInRange = playerIsInRange;
}

void Interactable::StopInteraction() {
	internalTimer = 0;
	audio->StopSoundsOnEvent(AUDIO_ON_INTERACTING);
}

bool Interactable::IsPlayerInRange() {
	DirectX::SimpleMath::Vector2 pos = trans->CurrentPos(), playerPos = g_GameManager->playerTrans->CurrentPos();

	return (pos - playerPos).LengthSquared() < interactDistanceSq;
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