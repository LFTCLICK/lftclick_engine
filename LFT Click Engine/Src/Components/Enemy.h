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
#include "Damageable.h"
#include "Component.h"
#include "GameObjectManager.h"
#include "Helpers.h"
#include <json.hpp>

#define DAYLIGHT_DEATH_TIME_MIN 0.f
#define DAYLIGHT_DEATH_TIME_MAX 10.f
#define DAYLIGHT_DEATH_CHANCE 0.9f

using json = nlohmann::json;
class Enemy : public Component
{
public:
	Enemy() : path(), ignoreWalls(false), wanderCircleRadius(1000.0f), targetPlayerRadius(50.0f), targetPosition() {};

	// Inherited via Component
	virtual void Start() override;
	void HandleMessage(Message* e);
	virtual void Update() override;
	virtual int getCompId() override { return ComponentType::ENEMY; };
	static int getStaticCompId() { return ComponentType::ENEMY; };
	virtual void Deserialize(nlohmann::json j, GameObject* componentOwner) override;

	virtual Component* Clone(GameObject* newParent);

public:
	float attackTimer, timer;
	float damage;
	float pathTimer=0;
	float wanderCircleRadius;
	float targetPlayerRadius;
	float speed;
	float aiSwitchDistance;
	bool switchToPlayer;
	bool ignoreWalls;
	DirectX::SimpleMath::Vector2 goal;
	DirectX::SimpleMath::Vector2 targetBeforePlayer;
	DirectX::SimpleMath::Vector2 targetPosition;
	DirectX::SimpleMath::Vector2 targetVector;

	std::list<DirectX::SimpleMath::Vector2> path;
	std::list<DirectX::SimpleMath::Vector2>::iterator currentPathPos;
	bool useObstacleAvoidance;
	DirectX::SimpleMath::Rectangle cabinRect = DirectX::SimpleMath::Rectangle(0 - 2280.0f/2, -2280.0f/2, 2280.0f, 2280.0f);
	bool doAstar;
	float reEvaluateStratTimer;
	float daylightDeathTime, daylightDeathChance;
	bool daylightDeathTriggered;

private:
	Transform* myTransform;
	Transform* playerTrans;
	float zHelper;
};
extern std::unique_ptr<AStarTerrain> g_AStarTerrain;