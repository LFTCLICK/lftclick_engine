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
#include <json.hpp>

using json = nlohmann::json;
class Bullet : public Component
{
public:

	// Inherited via Component
	virtual void Start() override;
	virtual void Update() override;
	virtual int getCompId() override { return ComponentType::BULLET; };
	virtual void Deserialize(nlohmann::json j, GameObject* parent) override;

	virtual Component* Clone(GameObject* newParent);
	Bullet() {};

public:
	DirectX::SimpleMath::Vector2 direction;
	bool liveForever;
	float animationTime, speed, timer, keepAliveTime;

private:
	GameObject* parent;
	Transform* trans;
};