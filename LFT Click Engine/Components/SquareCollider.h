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
#include "../EventManager.h"

using json = nlohmann::json;

class SquareCollider : public Collider {
public:
	// Inherited via Component
	SquareCollider();
	void Start();
	void Update();
	int getCompId();
	DirectX::XMFLOAT4 center;
	float width;
	float height;

	Component* clone(GameObject* newParent);
	void CollisionCheck(GameObject* toCheck);
	float* getPoints();
	SquareCollider(json j, GameObject* parent);

private:
	GameObject* parent;
	float* points;
	bool deleteOnCollison;
};