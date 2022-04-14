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
#include "GameObjectManager.h"
#include "InputManager.h"
#include "Transform.h"
#include "Camera.h"
#include "Component.h"
#include <json.hpp>

using json = nlohmann::json;
class FollowMouse : public Component
{
public:

	// Inherited via Component
	virtual void Start() override;
	virtual void Update() override;
	virtual int getCompId() override { return ComponentType::FOLLOW_MOUSE; };
	static int getStaticCompId() { return ComponentType::FOLLOW_MOUSE; };
	virtual Component* Clone(GameObject* newParent);

	FollowMouse() : gom(g_GameObjManager.get()) {}

	void Deserialize(nlohmann::json j, GameObject* componentOwner);

public:
	~FollowMouse();

private:
	GameObjectManager* gom;
	Transform* myTransform;
};