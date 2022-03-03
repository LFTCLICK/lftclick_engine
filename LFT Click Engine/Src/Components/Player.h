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
	Player() : isDashing(false), autopilot(false) {};
	virtual void Deserialize(nlohmann::json j, GameObject* parent) override;

	void HandleMessage(Message* e);

public:
	void Move(float deltaX, float deltaY);
	void Dash();
	void Sidescroll(float deltaTime);
	bool IsAutopilot() { return autopilot; }
private:
	GameObject* parent;
	Transform* trans;
	Camera* cam;
	Gun* gun;
	DirectX::SimpleMath::Vector2 dashVelocity;
	float playerSpeed, dashSpeedMultiplier, dashTime, dashTimer, deadZone = 8000;
	bool isDashing, autopilot;
};
