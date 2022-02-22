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
#include "../GameManager.h"

using json = nlohmann::json;
Camera::Camera()
{
	viewMatrix = DirectX::XMMatrixIdentity();
}

//Camera::Camera(json j, GameObject * parent)
//{
//	xPos = j["startX"];
//	yPos = j["startY"];
//	zPos = j["startZ"];
//	this->parent = parent;
//	speed = startingSpeed;
//	if (j["isMainCamera"])
//		GameManager::getInstance().mainCamera = this;
//}

Component * Camera::Clone(GameObject * newParent)
{
	Camera* toReturn = new Camera();
	toReturn->xPos = xPos;
	toReturn->yPos = yPos;
	toReturn->zPos = zPos;
	toReturn->xRot = xRot;
	toReturn->yRot = yRot;
	toReturn->zRot = zRot;
	toReturn->parent = newParent;
	return (Component*)toReturn;
}

void Camera::Deserialize(nlohmann::json j, GameObject* parent)
{
	xPos = j["startX"];
	yPos = j["startY"];
	zPos = j["startZ"];
	this->parent = parent;
	speed = startingSpeed;
	if (j["isMainCamera"])
		GameManager::getInstance().mainCamera = this;
}

void Camera::Start()
{
	xRot = 0;
	yRot = 0;
	zRot = 0;
}

void Camera::Update()
{
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
	return translation * DirectX::XMMatrixOrthographicLH(Graphics::getInstance().getWidth(), Graphics::getInstance().getHeight(), 0.00001f, 1000);
}

int Camera::getCompId()
{
	return CAMERA;
}


