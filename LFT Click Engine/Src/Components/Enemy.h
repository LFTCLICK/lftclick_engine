#pragma once
// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	Enemy.h
// Author			:	Rohit Punjabi
// Creation Date	:	2022/3/2
// Purpose			:	
// History			:
// ---------------------------------------------------------------------------

#include "GameObject.h"
#include "Component.h"
#include <json.hpp>
#include "Drawable.h"
#include "Camera.h"
#include "Gun.h"

using json = nlohmann::json;
class Enemy : public Component
{
public:

	// Inherited via Component
	virtual void Start() override;
	virtual void Update() override;
	virtual int getCompId() override { return ComponentType::PLAYER; };

	virtual Component* Clone(GameObject* newParent);
	Enemy() {};
	virtual void Deserialize(nlohmann::json j, GameObject* parent) override;

	void HandleMessage(Message* e);
	float RandomGenerator(float lowest, float highest);
private:
	GameObject* parent;
	Transform* myTransform;
	Gun* gun;
	int bulletCount;
	float timer; 
	float radius;
	float enemySpeed;

	DirectX::SimpleMath::Vector2 targetPosition;
};