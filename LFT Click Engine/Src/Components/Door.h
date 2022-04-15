#pragma once
// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	Door.h
// Author			:	Vance Howald
// Creation Date	:	2021/12/10
// Purpose			:	Door object to keep zombies out
// History			:
// ---------------------------------------------------------------------------

#include "GameObject.h"
#include "Transform.h"
#include "Component.h"
#include "GameObjectManager.h"
#include "SquareCollider.h"
#include "Drawable.h"
#include "Player.h"
#include <json.hpp>

using json = nlohmann::json;
class Door : public Component
{
public:

	// Inherited via Component
	virtual void Start() override;
	virtual void Update() override;
	virtual int getCompId() override { return ComponentType::DOOR_OBJ; };
	static int getStaticCompId() { return ComponentType::DOOR_OBJ; };
	virtual void Deserialize(nlohmann::json j, GameObject* componentOwner) override;
	virtual Component* Clone(GameObject* newParent);
	void UpdateImage();
	Door() {};

	void HandleMessage(Message* e);
public:
	int doorPhases, zeroIndexDoorPhases;
	float maxHp, hp;
	int woodRequiredPerPhase;
	float repairTime;

private:
	Transform* trans;
	SquareCollider* sqCollider;
	Drawable* draw;
	Player* p;
	bool playerInRange;
	bool playerExited;
	float internalTimer;
	int currentPhase, hpPerPhase;

};