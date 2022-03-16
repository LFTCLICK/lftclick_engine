// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	Camera.h
// Author			:	Vance Howald
// Creation Date	:	2021/11/14
// Purpose			:	Camera object header
// History			: 
// ---------------------------------------------------------------------------
#pragma once

#include "GameObject.h"
#include "Component.h"
#include "Transform.h"
#include <json.hpp>
#include <DirectXMath.h>

#define AUTOPILOT_START_DISTANCE 240

using json = nlohmann::json;
class Camera : public Component
{
public:
	Camera();

	// Inherited via Component
	virtual void Start() override;
	virtual void Update() override;
	virtual int getCompId() override;
	virtual Component* Clone(GameObject* newParent);
	virtual void Deserialize(nlohmann::json j, GameObject* componentOwner) override;
	

	void SetPos(float x, float y, float z);
	void SetRot(float x, float y, float z);

	void Move(float x, float y, float z = 0.0f);

	void SetAutopilotVelocity(std::string direction = "none", float speed = 0);

	DirectX::XMVECTOR getPos();
	DirectX::XMVECTOR getRot();

	void Render();
	void GetViewMatrix(DirectX::XMMATRIX& toReturn);
	DirectX::XMMATRIX GetProjectionMatrix();


	DirectX::SimpleMath::Vector2 WorldToScreenPos(DirectX::SimpleMath::Vector2 mycoords, float w, float h)
	{
		return DirectX::SimpleMath::Vector2((w / 2.0f) + (mycoords.x - xPos), (h / 2.0f) - (mycoords.y - yPos));
	}

	DirectX::SimpleMath::Vector2 ToScreenPos(DirectX::SimpleMath::Vector2 mycoords, float w, float h)
	{
		return DirectX::SimpleMath::Vector2((w / 2.0f) + mycoords.x, (h / 2.0f) - mycoords.y);
	}
	
public:
	float xPos, yPos, zPos, xRot, yRot, zRot;
	float maxSpeed;
private:
	float speed, speedDelta, startingSpeed, autopilotSpeed;
	DirectX::XMMATRIX viewMatrix;
	std::string autopilotDirection;
	bool isAutopilot;
};