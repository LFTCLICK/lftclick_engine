// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	MessCollider.h
// Author			:	Vance Howald
// Creation Date	:	2021/10/28
// Purpose			:	Mesh collision handling header
// ---------------------------------------------------------------------------
#pragma once
#include "Collider.h"
#include <json.hpp>
#include <vector>
#include <DirectXMath.h>
#include "Transform.h"
#include "EventManager.h"

using json = nlohmann::json;

class MeshCollider : public Collider {
public:
	// Inherited via Component
	MeshCollider();
	virtual void Deserialize(nlohmann::json j, GameObject* parent) override;

	virtual void Start();
	virtual void Update();
	int getCompId();

	virtual Component* Clone(GameObject* newParent);
	void CollisionCheck(GameObject* toCheck);
private:
	GameObject* parent;
	Transform* trans;
	std::vector<DirectX::XMVECTOR> vertices;
	float* pointArray;
	int arraySize;
	bool arrayResized;
};