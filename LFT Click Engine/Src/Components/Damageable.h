#pragma once
// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	AnimationHelper.h
// Author			:	Vance Howald
// Creation Date	:	2021/12/10
// Purpose			:	Animates grabables
// History			: 
// ---------------------------------------------------------------------------

#include "GameObject.h"
#include "Transform.h"
#include "Component.h"
#include "GameObjectManager.h"
#include "EventManager.h"
#include <json.hpp>

using json = nlohmann::json;
class Damageable : public Component
{
public:

	// Inherited via Component
	virtual void Start() override;
	virtual void Update() override;
	virtual int getCompId() override { return ComponentType::DAMAGEABLE; };
	virtual void Deserialize(nlohmann::json j, GameObject* parent) override;
	virtual void HandleMessage(Message* e) override;

	virtual Component* Clone(GameObject* newParent);
	Damageable() : destroyOnDeath(true), health(1) {};

public:
	int health;

	// For knockback
	DirectX::SimpleMath::Vector2 direction;
	float speed, acceleration;
	bool destroyOnDeath;

private:
	GameObject* parent;
	Transform* trans;
};