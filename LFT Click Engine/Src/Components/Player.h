#pragma once
// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	Player.h
// Author			:	Vance Howald
// Creation Date	:	2021/2/13
// Purpose			:	Main player
// History			:
// ---------------------------------------------------------------------------

#include "GameObject.h"
#include "Component.h"
#include <json.hpp>
#include "Drawable.h"
#include "Camera.h"
#include "Gun.h"
#include "AudioManager.h"
#include "FrameRateController.h"
#include "../LuaManager.h"
#include "Components/Collider.h"
#include <sol/sol.hpp>
#include <cassert>

using json = nlohmann::json;
class Player : public Component
{
public:

	// Inherited via Component
	virtual void Start() override;
	virtual void Update() override;
	virtual int getCompId() override { return ComponentType::PLAYER; };
	static int getStaticCompId() { return ComponentType::PLAYER; };

	virtual Component* Clone(GameObject* newParent);
	Player() : isDashing(false), dashTime(0.2), damageCooldownTimer(2.f), dashTimer(0.0f), wood(0) {};
	virtual void Deserialize(nlohmann::json j, GameObject* parent) override;

	void HandleMessage(Message* e);

public:
	void Move(float deltaX, float deltaY);
	void Dash();
	//void Sidescroll(float deltaTime);
	void ChangePlayerState();

	int wood, hp;

	//initializing LUA state
	std::string script;
	sol::state lua_player_state;
	sol::load_result player_script_update;

private:
	Transform* trans;
	Camera* cam;
	Gun* gun;

	DirectX::SimpleMath::Vector2 dashVelocity;
	float damageCooldownTimer, dashSpeed, dashTime, dashTimer;
	bool isDashing;
};