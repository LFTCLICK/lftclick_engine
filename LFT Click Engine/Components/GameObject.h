// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	GameObject.h
// Author			:	Vance Howald
// Creation Date	:	2021/10/28
// Purpose			:	Virtual functions for GameObject class
// History			:
// 2021/12/01		-	Added messaging
// ---------------------------------------------------------------------------
#pragma once
#include "Component.h"
#include "../Messages.h"
#include <list>

class GameObject
{
public:
	GameObject();
	void Update();
	void Start();
	void Add(Component* c);
	void Remove(Component* c);
	void HandleMessage(Message* e);
	Component* hasComp(Component::ComponentType comType);
	~GameObject();
public:
	std::string tag;
	bool isActive;
private:
	std::list<Component*> comps;
};