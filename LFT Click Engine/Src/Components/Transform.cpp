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

Transform::Transform() : isMoving(false), wasMoving(false), matchPlayerPos(false)
{
}

void Transform::Deserialize(nlohmann::json j, GameObject* componentOwner)
{
	this->componentOwner = componentOwner;

	position = DirectX::SimpleMath::Vector2(0.0f, 0.0f);
	position.x = j["startX"];
	position.y = j["startY"];
	scale.x = j["scaleX"];
	scale.y = j["scaleY"];
	rotation = j["rot"];
	zPos = j["zPos"];

	if (j.contains("matchPlayerPos")) 
		matchPlayerPos = j["matchPlayerPos"];

	componentOwner->trans = this;
}

Component * Transform::Clone(GameObject* newParent)
{
	Transform* toReturn = new Transform();

	toReturn->componentOwner = newParent;
	toReturn->position = position;
	toReturn->oldPos = oldPos;
	toReturn->lastMovement = lastMovement;
	toReturn->scale = scale;
	toReturn->rotation = rotation;
	toReturn->m = m;
	toReturn->zPos = zPos;
	toReturn->isMoving = isMoving;
	toReturn->wasMoving = wasMoving;
	toReturn->matchPlayerPos = matchPlayerPos;
	newParent->trans = toReturn;
	return (Component*)toReturn;
}

void Transform::Init(DirectX::SimpleMath::Vector2 const & v)
{
	position = DirectX::SimpleMath::Vector2(v);
}

void Transform::Move(float deltaX, float deltaY)
{
	position.x += deltaX;
	position.y += deltaY;
}

void Transform::SetPos(float newX, float newY)
{
	position.x = newX;
	position.y = newY;
}
void Transform::SetPos(const DirectX::SimpleMath::Vector2& position)
{
	this->position = position;
}
void Transform::SetPos(float newX, float newY, float newZ)
{
	position.x = newX;
	position.y = newY;
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
	if (wasMoving != isMoving) wasMoving = isMoving;

	if (position.x != oldPos.x || position.y != oldPos.y) {
		if (!isMoving) isMoving = true;
		lastMovement.x = position.x - oldPos.x;
		lastMovement.y = position.y - oldPos.y;
		oldPos.x = position.x;
		oldPos.y = position.y;
	}
	else if (isMoving) {
		lastMovement.x = lastMovement.y = 0;
		isMoving = false;
	}

	if (matchPlayerPos) SetPos(g_GameManager->playerTrans->CurrentPos());
}

void Transform::HandleMessage(Message* e)
{
}

DirectX::SimpleMath::Vector2 Transform::CurrentPos()
{
	return position;
}

DirectX::XMMATRIX Transform::GetXMMatrix()
{
	return DirectX::XMMatrixScaling(scale.x, scale.y, 1) *
		DirectX::XMMatrixRotationRollPitchYaw(0, 0, rotation) *
		DirectX::XMMatrixTranslation(position.x, position.y, zPos);
}

Transform::~Transform()
{
}