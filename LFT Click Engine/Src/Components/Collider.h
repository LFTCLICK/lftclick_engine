// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	Collider.h
// Author			:	Vance Howald
// Creation Date	:	2021/12/01
// Purpose			:	Base class for other colliders
// History			: 
// ---------------------------------------------------------------------------
#pragma once
#include "Component.h"
#include "GameObject.h"

class Collider : public Component {
public:
	// Inherited via Component
	virtual void Start() = 0;
	virtual void Update() = 0;
	virtual int getCompId() = 0;

	virtual Component* Clone(GameObject* newParent) = 0;
	virtual void CollisionCheck(GameObject* toCheck) = 0;
protected:
	bool isTrigger;

};