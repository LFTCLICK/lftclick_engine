#pragma once
// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	SpriteAnimator.h
// Author			:	Vance Howald
// Creation Date	:	2021/2/13
// Purpose			:	Animator
// History			:
// ---------------------------------------------------------------------------

#include "GameObject.h"
#include "Component.h"
#include <json.hpp>
#include "Drawable.h"
#include "Camera.h"
#include "Gun.h"

using json = nlohmann::json;
class Player : public Component
{
public:

	// Inherited via Component
	virtual void Start() override;
	virtual void Update() override;
	virtual int getCompId() override { return ComponentType::PLAYER; };

	virtual Component* Clone(GameObject* newParent);
	Player() {};
	Player(json j, GameObject* parent);

	void HandleMessage(Message* e);

private:
	GameObject* parent;
	Transform* myTransform;
	Camera* cam;
	Gun* gun;
	float playerSpeed;

	float deadZone = 8000;
};
