// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	Component.h
// Author			:	Vance Howald
// Creation Date	:	2021/11/14
// Purpose			:	Header for component objects
// History			:
// 2021/12/01		-	Added messaging
// ---------------------------------------------------------------------------
#pragma once
#include "../Messages.h"

class GameObject;

class Component
{
public:
	enum ComponentType{asda, TRANSFORM, DRAWABLE, MESH_COLLIDER, SQUARE_COLLLIDER, CAMERA, ANIMATION_HELPER};
	Component();
	virtual void Start() = 0;
	virtual void Update() = 0;
	virtual Component* Clone(GameObject* newParent) = 0;
	virtual int getCompId() = 0;
	virtual void HandleMessage(Message* e) {};
	~Component();

public:
};