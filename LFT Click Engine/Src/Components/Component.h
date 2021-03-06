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
	enum ComponentType {
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
		CIRCLE_COLLIDER,
		INTERACTABLE,
		END_OF_ENGINE_COMPS = 100,
		ENEMY_SPAWNER,
		DOOR_OBJ,
		ENEMY,
		WOOD_PILE,
		VORTEX,
		CREDITS,
		SIDESCROLL_COMPS = 300,
		LEVEL_GENERATOR
	};

	Component() : componentOwner(nullptr) {};
	virtual ~Component() = default;

	virtual void Start() {};//called after the gameobject is made/cloned, comps are called in order of the ComponentType enum. But don't rely on this please
	virtual void Update() {};
	virtual void HandleMessage(Message* e) {};

	virtual void Deserialize(nlohmann::json j, GameObject* componentOwner) = 0;
	virtual int getCompId() = 0;
	virtual Component* Clone(GameObject* newParent) = 0;

public:
	GameObject* componentOwner;
	bool isFinishedDeleting = true;
};
