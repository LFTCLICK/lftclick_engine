#pragma once
#include "Collider.h"
#include "GameObject.h"
#include <json.hpp>
#include <DirectXMath.h>
#include <vector>
#include "EventManager.h"

using json = nlohmann::json;

class CircleCollider : public Collider {
public:
	// Inherited via Component
	CircleCollider();
	virtual void Start();
	virtual void Update();
	virtual int getCompId() override {return ComponentType::CIRCLE_COLLIDER;};
	static int getStaticCompId() {return ComponentType::CIRCLE_COLLIDER;};
	DirectX::SimpleMath::Vector2 center;
	float radius;

	virtual Component* Clone(GameObject* newParent);
	void CollisionCheck(GameObject* toCheck);
	virtual void Deserialize(nlohmann::json j, GameObject* componentOwner) override;

	void DebugDraw() override;
private:
	bool deleteOnCollison;
};