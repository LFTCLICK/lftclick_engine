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
	enum ComponentType{
		TRANSFORM = 1, 
		DRAWABLE, 
		MESH_COLLIDER, 
		SQUARE_COLLLIDER, 
		CAMERA, 
		SPRITE_ANIMATOR, 
		PLAYER, 
		AUDIBLE, 
		BULLET, 
		GUN, 
		FOLLOW_MOUSE, 
		DAMAGEABLE,
		END_OF_ENGINE_COMPS = 100
	};

	Component();
	virtual void Start() = 0;//called after the gameobject is made/cloned, comps are called in order of the ComponentType enum. But don't rely on this please
	virtual void Update() = 0;//called every frame
	virtual void Deserialize(nlohmann::json j, GameObject* parent) = 0;
	virtual Component* Clone(GameObject* newParent) = 0;//basically a copy constructor
	virtual int getCompId() = 0;//needs to return its ComponentType enum
	virtual void HandleMessage(Message* e) {};//message handling
	~Component();

public:
};
