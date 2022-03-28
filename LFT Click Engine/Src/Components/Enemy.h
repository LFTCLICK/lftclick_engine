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
#include "Component.h"
#include "GameObjectManager.h"
#include <json.hpp>

using json = nlohmann::json;
class Enemy : public Component
{
public:

	// Inherited via Component
	virtual void Start() override;
	void HandleMessage(Message* e);
	virtual void Update() override;
	virtual int getCompId() override { return ComponentType::ENEMY; };
	static int getStaticCompId() { return ComponentType::ENEMY; };
	virtual void Deserialize(nlohmann::json j, GameObject* componentOwner) override;

	virtual Component* Clone(GameObject* newParent);
	Enemy():path(){};

public:
	float attackTimer, timer;
	float damage;
	float pathTimer=0;
	float speed;
	float aiSwitchDistance;
	bool hanginWithTheHomies;
	bool switchToPlayer;
	DirectX::SimpleMath::Vector2 goal;
	DirectX::SimpleMath::Vector2 targetBeforePlayer;
	std::list<DirectX::SimpleMath::Vector2> path;
	std::list<DirectX::SimpleMath::Vector2>::iterator currentPathPos;

private:
	Transform* trans;
	Transform* playerTrans;
};
extern std::unique_ptr<AStarTerrain> g_AStarTerrain;