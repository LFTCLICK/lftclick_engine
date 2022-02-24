#pragma once
// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	Gun.h
// Author			:	Chris Fitch
// Creation Date	:	2022/12/10
// Purpose			:	Contains functions for firing bullets
// History			: 
// ---------------------------------------------------------------------------

#include "GameObject.h"
#include "GameObjectFactory.h"
#include "GameObjectManager.h"
#include "InputManager.h"
#include "Bullet.h"
#include "Transform.h"
#include "Component.h"
#include <json.hpp>
#include <map>
#include <iostream>

using json = nlohmann::json;
class Gun : public Component
{
public:

	// Inherited via Component
	virtual void Start() override;
	virtual void Update() override;
	virtual int getCompId() override { return ComponentType::GUN; };

	virtual Component* Clone(GameObject* newParent);

	Gun() {}
	Gun(json j, GameObject* parent);

public:
	~Gun();
	void Fire(int bulletIndex, float targetX, float targetY);

private:
	GameObject* parent;
	Transform* trans;

	GameObjectManager* gom;
	GameObjectFactory* gof;

	float timer;
	int nextBulletID;
	typedef std::pair<GameObject*, float> BulletData; // holds bullet prototype and rate of fire
	std::map<int, BulletData> bulletTypes;
};