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
#include "Graphics.h"

using json = nlohmann::json;
Camera::Camera()
{
	viewMatrix = DirectX::XMMatrixIdentity();
}

Camera::Camera(json j, GameObject * parent)
{
	startingSpeed = j["speed"];
	xPos = j["startX"];
	yPos = j["startY"];
	zPos = j["startZ"];
	speedDelta = j["speedDelta"];
	maxSpeed = j["maxSpeed"];
	this->parent = parent;
	speed = startingSpeed;
}

Component * Camera::Clone(GameObject * newParent)
{
	Camera* toReturn = new Camera();
	toReturn->xPos = xPos;
	toReturn->yPos = yPos;
	toReturn->zPos = zPos;
	toReturn->xRot = xRot;
	toReturn->yRot = yRot;
	toReturn->zRot = zRot;
	toReturn->speed = speed;
	toReturn->speedDelta = speedDelta;
	toReturn->startingSpeed = startingSpeed;
	toReturn->parent = newParent;
	return (Component*)toReturn;
}

void Camera::Start()
{
	xRot = 0;
	yRot = 0;
	zRot = 0;
}

void Camera::Update()
{
	yPos += speed * FrameRateController::getInstance().DeltaTime();
	speed += speedDelta * FrameRateController::getInstance().DeltaTime();
	if (speed >= maxSpeed)
		speed = maxSpeed;
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
	viewMatrix = DirectX::XMMatrixTranslation(-xPos, -yPos, zPos) * DirectX::XMMatrixRotationRollPitchYaw(xRot, yRot, zRot) *  DirectX::XMMatrixIdentity();
}

void Camera::GetViewMatrix(DirectX::XMMATRIX & toReturn)
{
	toReturn = DirectX::XMMatrixTranslation(-xPos, -yPos, zPos) * DirectX::XMMatrixRotationRollPitchYaw(xRot, yRot, zRot) *  DirectX::XMMatrixIdentity();
	//toReturn = viewMatrix;
}

DirectX::XMMATRIX Camera::GetProjectionMatrix()
{
	DirectX::XMMATRIX translation;
	GetViewMatrix(translation);
	return translation * DirectX::XMMatrixOrthographicLH(Graphics::getInstance().GetWidth(), Graphics::getInstance().GetHeight(), 0.00001f, 1000);
}

int Camera::getCompId()
{
	return CAMERA;
}

