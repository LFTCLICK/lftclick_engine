// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	WoodPile.h
// Author			:	Vance Howald
// Creation Date	:	2021/11/14
// Purpose			:	Pickups for door repairs
// History			: 
// ---------------------------------------------------------------------------
#pragma once

#include "GameObject.h"
#include "Component.h"
#include "Transform.h"
#include "Player.h"
#include <json.hpp>


using json = nlohmann::json;
class WoodPile : public Component
{
public:

	// Inherited via Component
	virtual void Start() override;
	virtual void Update() override;
	virtual int getCompId() override { return ComponentType::WOOD_PILE; };
	virtual Component* Clone(GameObject* newParent);
	virtual void Deserialize(nlohmann::json j, GameObject* componentOwner) override;

	void HandleMessage(Message* e);

	WoodPile() {};


public:
	int woodPerCollect;
	float timeToCollect;

private:
	Transform* trans;
	//SquareCollider* sqCollider;
	//Drawable* draw;
	Player* p;
	bool playerInRange;
	bool playerExited;
	float internalTimer;
	int currentPhase, hpPerPhase;
};