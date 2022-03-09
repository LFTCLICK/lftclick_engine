// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	SquareCollider.h
// Author			:	Vance Howald
// Creation Date	:	2021/11/14
// Purpose			:	Square collider handling
// History			:
// ---------------------------------------------------------------------------
#pragma once
#include "Collider.h"
#include "GameObject.h"
#include <json.hpp>
#include <DirectXMath.h>
#include <vector>
#include "EventManager.h"

using json = nlohmann::json;

class SquareCollider : public Collider {
public:
	// Inherited via Component
	SquareCollider();
	virtual void Start();
	virtual void Update();
	int getCompId();
	DirectX::XMFLOAT4 center;
	float width;
	float height;

	virtual Component* Clone(GameObject* newParent);
	void CollisionCheck(GameObject* toCheck);
	float* getPoints();
	virtual void Deserialize(nlohmann::json j, GameObject* parent) override;

	void DebugDraw() override;
private:
	GameObject* parent;
	float* points;
	bool deleteOnCollison;
};