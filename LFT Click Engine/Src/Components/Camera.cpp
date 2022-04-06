// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	Camera.cpp
// Author			:	Vance Howald
// Creation Date	:	2021/11/14
// Purpose			:	implementation of Camera object
// History			:
// ---------------------------------------------------------------------------
#include "pch.h"
#include "Camera.h"
#include "FrameRateController.h"
#include "Renderer.h"
#include "../GameManager.h"

using json = nlohmann::json;

Camera::Camera()
{
	viewMatrix = DirectX::XMMatrixIdentity();
}

Component* Camera::Clone(GameObject* newParent)
{
	Camera* toReturn = new Camera();
	toReturn->xPos = xPos;
	toReturn->yPos = yPos;
	toReturn->zPos = zPos;
	toReturn->xRot = xRot;
	toReturn->yRot = yRot;
	toReturn->zRot = zRot;
	toReturn->isAutopilot = isAutopilot;
	toReturn->autopilotDirection = autopilotDirection;
	toReturn->autopilotSpeed = autopilotSpeed;
	toReturn->componentOwner = newParent;
	return (Component*)toReturn;
}

void Camera::Deserialize(nlohmann::json j, GameObject* componentOwner)
{
	xPos = j["startX"];
	yPos = j["startY"];
	zPos = j["startZ"];
	this->componentOwner = componentOwner;
	speed = startingSpeed;
	if (j["isMainCamera"])
		g_GameManager->mainCamera = this;
}

void Camera::Start()
{
	xRot = 0;
	yRot = 0;
	zRot = 0;
	trans = componentOwner->getComponent<Transform>();
}

void Camera::Update()
{
	if (isAutopilot) {
		float movement = autopilotSpeed * g_FrameRateController->DeltaTime();

		if (autopilotDirection == "right") xPos += movement;
		else if (autopilotDirection == "left") xPos -= movement;
		else if (autopilotDirection == "up") yPos += movement;
		else if (autopilotDirection == "down") yPos -= movement;
	}
	else if (componentOwner->getComponent<Transform>() != nullptr) {
		auto pos = componentOwner->getComponent<Transform>()->CurrentPos();
		xPos = pos.x;
		yPos = pos.y;
	}
}

void Camera::SetPos(float x, float y, float z)
{
	xPos = x;
	yPos = y;
	zPos = z;
}

void Camera::SetRot(float x, float y, float z)
{
	xRot = x;
	yRot = y;
	zRot = z;
}

void Camera::Move(float x, float y, float z)
{
	xPos += x;
	yPos += y;
	zPos += z;
}



void Camera::SetAutopilotVelocity(std::string direction, float speed) 
{
	isAutopilot = true;
	autopilotSpeed = speed;
	autopilotDirection = direction;

	auto pos = trans->CurrentPos();
	xPos = pos.x, yPos = pos.y;

	if (autopilotDirection != "" && autopilotDirection != "none") {
		if (autopilotDirection == "right") xPos += AUTOPILOT_START_DISTANCE;
		else if (autopilotDirection == "left") xPos -= AUTOPILOT_START_DISTANCE;
		else if (autopilotDirection == "up") yPos += AUTOPILOT_START_DISTANCE;
		else if (autopilotDirection == "down") yPos -= AUTOPILOT_START_DISTANCE;
	}
}

DirectX::XMVECTOR Camera::getPos()
{
	return DirectX::XMVECTOR{ xPos, yPos, zPos, 1 };
}

DirectX::XMVECTOR Camera::getRot()
{
	return DirectX::XMVECTOR{ xRot, yRot, zRot, 1 };
}

void Camera::Render()
{
	viewMatrix = DirectX::XMMatrixTranslation(-xPos, -yPos, zPos) * DirectX::XMMatrixRotationRollPitchYaw(xRot, yRot, zRot);
}

DirectX::XMMATRIX Camera::GetViewMatrix()
{
	return DirectX::XMMatrixTranslation(-xPos, -yPos, zPos) * DirectX::XMMatrixRotationRollPitchYaw(xRot, yRot, zRot);
}

DirectX::XMMATRIX Camera::GetProjectionMatrix()
{
	return GetViewMatrix() * DirectX::XMMatrixOrthographicLH(g_Renderer->GetWidth(), g_Renderer->GetHeight(),
		0.1f, 1000);
}