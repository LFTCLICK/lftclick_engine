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
class Component
{
public:
	enum ComponentType{PLAYER, TRANSFORM, DRAWABLE, MESH_COLLIDER, SQUARE_COLLLIDER, CAMERA, LEVEL_SPAWNING, ANIMATION_HELPER};
	Component();
	virtual void Start() = 0;
	virtual void Update() = 0;
	virtual int getCompId() = 0;
	virtual void HandleMessage(Message* e) {};
	~Component();

public:
};