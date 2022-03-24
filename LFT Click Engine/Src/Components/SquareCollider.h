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
#include "Camera.h"

using json = nlohmann::json;

class SquareCollider : public Collider {
public:
	// Inherited via Component
	SquareCollider();
	virtual void Start();
	virtual void Update();
	virtual int getCompId() override {return ComponentType::SQUARE_COLLLIDER;};
	static int getStaticCompId() {return ComponentType::SQUARE_COLLLIDER;};
	DirectX::SimpleMath::Vector2 center;
	float width;
	float height;

	virtual Component* Clone(GameObject* newParent);
	void CollisionCheck(GameObject* toCheck);
	float* getPoints();
	virtual void Deserialize(nlohmann::json j, GameObject* componentOwner) override;

	void DebugDraw() override;
private:
	float* points;
	bool deleteOnCollison;
};