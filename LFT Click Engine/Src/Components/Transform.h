// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	Transform.h
// Author			:	Vance Howald
// Creation Date	:	2021/10/28
// Purpose			:	Position info for object
// History			:
// 2021/12/01		-	Added messaging and get vector
// ---------------------------------------------------------------------------
#pragma once

#include "GameObject.h"
#include <json.hpp>
#include <DirectXMath.h>
#include "EventManager.h"

using json = nlohmann::json;

class Transform : public Component {
public:
	Transform();
	virtual void Deserialize(nlohmann::json j, GameObject* componentOwner) override;
	virtual void Start();
	virtual void Update();
	virtual int getCompId();
	virtual Component* Clone(GameObject* newParent);
	void HandleMessage(Message* e);
	~Transform();

public:
	void Init(DirectX::SimpleMath::Vector2 const& v);
	void Move(float deltaX, float deltaY);
	void SetPos(float newX, float newY);
	void SetPos(const DirectX::SimpleMath::Vector2& position);

	void SetPos(float newX, float newY, float newZ);
	void Rotate(float angleDelta);
	DirectX::SimpleMath::Vector2 CurrentPos();
	
	DirectX::XMMATRIX GetXMMatrix();

	DirectX::SimpleMath::Vector2 position, oldPos, lastMovement, scale;
	bool isMoving, wasMoving;

private:
	float rotation, zPos;
	DirectX::XMFLOAT4X4 m;
	DirectX::XMVECTOR v;
};
