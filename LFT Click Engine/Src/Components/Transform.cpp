// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	Transform.cpp
// Author			:	Vance Howald
// Creation Date	:	2021/10/28
// Purpose			:	Position info for object
// History			:
// 2021/12/01		-	GetPosXMVector added
// ---------------------------------------------------------------------------
#include "pch.h"
#include "Transform.h"

Transform::Transform()
{
}

Transform::Transform(json j, GameObject * parent)
{
	this->parent = parent;
	pos = DirectX::SimpleMath::Vector2(0.0f, 0.0f);
	pos.x = j["startX"];
	pos.y = j["startY"];
	scale.x = j["scaleX"];
	scale.y = j["scaleY"];
	rotation = j["rot"];
	zPos = j["zPos"];
}

Component * Transform::Clone(GameObject* newParent)
{
	Transform* toReturn = new Transform();
	toReturn->parent = newParent;
	toReturn->pos = pos;
	toReturn->scale = scale;
	toReturn->rotation = rotation;
	toReturn->m = m;
	toReturn->zPos = zPos;
	return (Component*)toReturn;
}

void Transform::Init(DirectX::SimpleMath::Vector2 const & v)
{
	pos = DirectX::SimpleMath::Vector2(v);
}

void Transform::Move(float deltaX, float deltaY)
{
	pos.x += deltaX;
	pos.y += deltaY;
}

void Transform::SetPos(float newX, float newY)
{
	pos.x = newX;
	pos.y = newY;
}
void Transform::SetPos(float newX, float newY, float newZ)
{
	pos.x = newX;
	pos.y = newY;
	zPos = newZ;
}

void Transform::Rotate(float angleDelta)
{
	rotation += angleDelta;
}

void Transform::Start()
{
}

void Transform::Update()
{
}

int Transform::getCompId()
{
	return Component::TRANSFORM;
}

void Transform::HandleMessage(Message * e)
{
}

DirectX::SimpleMath::Vector2 Transform::CurrentPos()
{
	return pos;
}

DirectX::XMFLOAT4X4 Transform::GetXMMatrix()
{
	DirectX::XMMATRIX temp = DirectX::XMMatrixScaling(scale.x, scale.y, 1) * DirectX::XMMatrixRotationRollPitchYaw(0, 0, rotation) * DirectX::XMMatrixTranslation(pos.x, pos.y, zPos) * DirectX::XMMatrixIdentity();
	XMStoreFloat4x4(&m, temp);
	//m = DirectX::XMMatrixTranslation(pos.x, pos.y, 0)*  DirectX::XMMatrixIdentity();
	return m;
}

DirectX::XMVECTOR Transform::GetPosXMVector()
{
	
	v = DirectX::XMVectorSet(pos.x, pos.y, 0.0, 1.0);
	return v;
}

Transform::~Transform()
{
}