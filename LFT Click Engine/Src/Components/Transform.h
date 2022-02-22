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
#include "../Vector2D.h"
#include "GameObject.h"
#include <json.hpp>
#include <DirectXMath.h>
#include "../EventManager.h"

using json = nlohmann::json;
class Transform : public Component {
public:
	Transform();
	virtual void Deserialize(nlohmann::json j, GameObject* parent) override;
	void Init(Vector2D const & v);
	void Move(float deltaX, float deltaY);
	void SetPos(float newX, float newY);
	void SetPos(float newX, float newY, float newZ);
	void Rotate(float angleDelta);
	virtual void Start();
	virtual void Update();
	virtual int getCompId();
	virtual Component* Clone(GameObject* newParent);
	void HandleMessage(Message* e);
	Vector2D CurrentPos();
	DirectX::XMFLOAT4X4 GetXMMatrix();
	DirectX::XMVECTOR GetPosXMVector();
	~Transform();
public:
	Vector2D scale;
private:
	GameObject * parent;
	Vector2D pos;
	float rotation;
	DirectX::XMFLOAT4X4 m;
	DirectX::XMVECTOR v;
	float zPos;
};