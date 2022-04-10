#pragma once
// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	Interactable.h
// Author			:	Chris Fitch
// Creation Date	:	2022/03/20
// Purpose			:	Makes object interactable
// History			: 
// ---------------------------------------------------------------------------

#include "GameObject.h"
#include "GameManager.h"
#include "Component.h"
#include "Transform.h"
#include "Audible.h"
#include "SpriteAnimator.h"
#include "Player.h"
#include "FrameRateController.h"
#include "Collider.h"
#include "Helpers.h"
#include <json.hpp>


using json = nlohmann::json;
class Interactable : public Component
{
public:

	// Inherited via Component
	virtual void Start() override;
	virtual void Update() override;
	virtual int getCompId() override { return ComponentType::INTERACTABLE; };
	virtual Component* Clone(GameObject* newParent);
	virtual void Deserialize(nlohmann::json j, GameObject* componentOwner) override;

	void HandleMessage(Message* e);

	Interactable() : currentPhase(0), totalPhases(4), currentHp(1), hpPerPhase(1), hasParts(false) {}


public:
	int woodPerCollect;
	float timeToCollect;

private:
	void StartInteraction();
	void StopInteraction();
	void ContinueInteraction();
	void CompleteInteraction();
	bool IsPlayerInRange();

private:
	Transform* myTransform;
	Audible* audio;
	SpriteAnimator* anim;
	Drawable* drawable;

	static bool tutorialUI;

	bool playerIsInRange, playerWasInRange, interacting, hasParts;
	float internalTimer, interactDistance, interactDistanceSq;
	int currentPhase, totalPhases, currentHp, hpPerPhase;
};