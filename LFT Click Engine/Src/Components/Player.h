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
#include "GameManager.h"
#include "FrameRateController.h"
#include "../LuaManager.h"
#include "Components/Collider.h"
#include <sol/sol.hpp>
#include <cassert>
#include "AStarTerrain.h"

class SquareCollider;


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
	Player() : isDashing(false), autopilot(false), damageFlashing(false), 
		dashTime(0.2), damageCooldownTimer(2.f), dashTimer(0.0f), wood(0), damageFlashTimer(0.0f) {};

	virtual void Deserialize(nlohmann::json j, GameObject* parent) override;

	void HandleMessage(Message* e);

public:
	void Move(float deltaX, float deltaY);
	void Dash();
	void DamagePlayer(); 
	void Sidescroll(float deltaTime);
	bool IsAutopilot() { return autopilot; }

	int wood, health, parts;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> woodSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> healthSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> bikepartsSRV;

	//initializing LUA state
	std::string script;
	sol::state lua_player_state;
	sol::load_result player_script_update;

private:
	Transform* myTransform;
	Camera* cam;
	Gun* gun;
	Drawable* drawable;
	SquareCollider* squareCollider;
	float introTimer;

	DirectX::SimpleMath::Vector2 dashVelocity;
	float damageCooldownTimer, dashSpeed, dashTime, dashTimer, playerSpeed, zHelper;
	bool isDashing, autopilot;
	
	bool damageFlashing;
	float damageFlashTimer;
};