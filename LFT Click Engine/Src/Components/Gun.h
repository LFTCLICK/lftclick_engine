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
	static int getStaticCompId() { return ComponentType::GUN; };

	virtual Component* Clone(GameObject* newParent);

	Gun() : gom(g_GameObjManager.get()), gof(g_GameObjFactory.get()), fireRateTimer(0), currentBulletIndex(0) {}
	virtual void Deserialize(nlohmann::json j, GameObject* componentOwner) override;

public:
	~Gun() {}
	bool ReadyToFire();
	void Fire(float targetX, float targetY);
	void SwitchBulletIndex(int bulletIndex);

private:
	Transform* myTransform;

	GameObjectManager* gom;
	GameObjectFactory* gof;

	float fireRateTimer;
	int currentBulletIndex;
	typedef std::pair<std::string, float> BulletData; // holds bullet prototype and rate of fire
	std::map<int, BulletData> bulletTypes;
};