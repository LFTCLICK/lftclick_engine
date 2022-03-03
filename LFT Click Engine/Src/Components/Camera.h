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

	// Inherited via Component
	virtual void Start() override;
	virtual void Update() override;
	virtual int getCompId() override;
	virtual Component* Clone(GameObject* newParent);
	virtual void Deserialize(nlohmann::json j, GameObject* parent) override;

	Camera();

	

	void SetPos(float x, float y, float z);
	void SetRot(float x, float y, float z);

	void Move(float x, float y, float z = 0.0f);

	void SetAutopilotVelocity(std::string direction = "none", float speed = 0);
	
	DirectX::XMVECTOR getPos();
	DirectX::XMVECTOR getRot();

	void Render();
	void GetViewMatrix(DirectX::XMMATRIX &toReturn);
	DirectX::XMMATRIX GetProjectionMatrix();

	float xPos, yPos, zPos, xRot, yRot, zRot;
	float maxSpeed;
private:
	float speed, speedDelta, startingSpeed, autopilotSpeed;
	GameObject* parent;
	DirectX::XMMATRIX viewMatrix;
	std::string autopilotDirection;
	bool isAutopilot;
};