#pragma once
// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	Interactable.h
// Author			:	Chris Fitch
// Creation Date	:	2022/03/20
// Purpose			:	Animates grabables
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

	Interactable() {}


public:
	int woodPerCollect;
	float timeToCollect;

private:
	void StopInteraction();
	bool IsPlayerInRange();

private:
	Transform* trans;
	Audible* audio;
	SpriteAnimator* anim;

	bool playerIsInRange, playerWasInRange, interacting;
	float internalTimer, interactDistance, interactDistanceSq;
	int currentPhase, hpPerPhase;
};