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
#include "Audible.h"
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
	
	Door() : repairing(false) {}

	void HandleMessage(Message* e);
public:
	int doorPhases, zeroIndexDoorPhases;
	float maxHp, playerHealth;
	int woodRequiredPerPhase;
	float repairTime;
	bool inWoodPilePhase;

private:
	Transform* myTransform;
	DirectX::SimpleMath::Vector2 woodPileOffset;
	float woodPileRadius;
	SquareCollider* squareCollider;
	Drawable* drawable;
	Player* player;
	bool playerInRange;
	bool playerExited;
	bool repairing;
	float internalTimer;
	int currentPhase, hpPerPhase;

};