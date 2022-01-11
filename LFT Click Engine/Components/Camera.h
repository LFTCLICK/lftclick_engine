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

using json = nlohmann::json;
class Camera : public Component
{
public:

	// Inherited via Component
	virtual void Start() override;
	virtual void Update() override;
	virtual int getCompId() override;

	Camera();
	Camera(json j, GameObject* parent);

	Component* clone(GameObject* newParent);
	

	void SetPos(float x, float y, float z);
	void SetRot(float x, float y, float z);
	
	DirectX::XMVECTOR getPos();
	DirectX::XMVECTOR getRot();

	void Render();
	void GetViewMatrix(DirectX::XMMATRIX &toReturn);
	DirectX::XMMATRIX GetProjectionMatrix();

	void SlowCamera();

	float xPos, yPos, zPos, xRot, yRot, zRot;
	float maxSpeed;
private:
	float speed, speedDelta, startingSpeed;
	GameObject* parent;
	DirectX::XMMATRIX viewMatrix;
};