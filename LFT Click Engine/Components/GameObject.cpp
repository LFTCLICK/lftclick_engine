// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	GameObject.cpp
// Author			:	Vance Howald
// Creation Date	:	2021/10/28
// Purpose			:	Virtual functions for GameObject class
// History			:
// 2021/12/01		:	Added messaging
// ---------------------------------------------------------------------------
#include "GameObject.h"

GameObject::GameObject()
{
	comps = std::list<Component*>();
	isActive = true;
}

void GameObject::Update()
{
	for (Component* c : comps)
		c->Update();
}

void GameObject::Start()
{
	for (Component* c : comps)
		c->Start();
}

void GameObject::Add(Component* c)
{
	comps.emplace_back(c);
}

void GameObject::Remove(Component * c)
{
	comps.remove(c);
}

void GameObject::HandleMessage(Message * e)
{
	for (Component* c : comps)
		c->HandleMessage(e);
}

Component * GameObject::hasComp(Component::ComponentType comType)
{
	for (Component* c : comps)
	{
		if (c->getCompId() == comType)
			return c;
	}
	return nullptr;
}

GameObject::~GameObject()
{
	/*for (Component* c : comps)
	{
		delete c;
	}*/
	comps.clear();
}
