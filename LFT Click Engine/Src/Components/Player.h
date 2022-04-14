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
#include "Audible.h"
#include "Camera.h"
#include "Gun.h"
#include "Enemy.h"
#include "AudioManager.h"
#include "GameManager.h"
#include "FrameRateController.h"
#include "../LuaManager.h"
#include "Components/Collider.h"
#include <sol/sol.hpp>
#include <cassert>
#include "AStarTerrain.h"

#define POSITIVE_BOUND_X 6630.f
#define POSITIVE_BOUND_Y 6630.f
#define NEGATIVE_BOUND_X -6630.f
#define NEGATIVE_BOUND_Y -6630.f

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
		dashTime(0.2), damageCooldownTimer(2.f), dashTimer(0.0f), collectibleWood(0),
		inertiaMod(60), hitDirection{0, 0}, hitSpeed(0) {};

	virtual void Deserialize(nlohmann::json j, GameObject* parent) override;

	void HandleMessage(Message* e);
public:
	void Move(float deltaX, float deltaY);
	void Dash();
	void DamagePlayer(); 
	void Sidescroll(float deltaTime);
	bool IsAutopilot() { return autopilot; }

	int collectibleWood, playerHealth, collectibleparts;
	
	//HUD animations
	bool playCollectedAnimWood = false;
	bool playCollectedAnimParts = false;

	DirectX::SimpleMath::Vector2 woodHUDPos = { 4, 20 };
	DirectX::SimpleMath::Vector2 bikeHUDPos = { 4, 50 };
	DirectX::SimpleMath::Vector2 healthHUDPos = { 4, 90 };

	float woodHUDScale = 2.6f; //Initial
	float currentWoodHUDScale = woodHUDScale; //changes as per animation
	float woodHUDTextScale = 1.0f; //Initial
	float currentWoodHUDTextScale = woodHUDTextScale; //changes as per animation

	float bikeHUDScale = 2.6f; //Initial
	float currentBikeHUDScale = bikeHUDScale; //changes as per animation
	float bikeHUDTextScale = 1.0f; //Initial
	float currentBikeHUDTextScale = bikeHUDTextScale; //changes as per animation
 	
	float healthHUDScale = 2.6f;


	DirectX::SimpleMath::Vector2 woodTextPos = { 50, 25 };
	DirectX::SimpleMath::Vector2 bikeTextPos = { 50, 60 };
	DirectX::SimpleMath::Vector2 healthTextPos = { 50, 100 };


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
	Audible* audio;
	SpriteAnimator* anim;
	SquareCollider* squareCollider;
	float introTimer, inertiaMod, hitSpeed;

	DirectX::SimpleMath::Vector2 dashVelocity, hitDirection, positiveBound, negativeBound;
	sol::function PlayerCollidedWithEnemy;
	TimedMessage currentMessage;
	float damageCooldownTimer, dashSpeed, dashTime, dashTimer, playerSpeed, zHelper, playerSpeedForSideScroller, camSpeed;
	bool isDashing, autopilot, playerInPuddle;
	
	bool damageFlashing;
};