// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	GameObject.cpp
// Author			:	Vance Howald
// Creation Date	:	2021/10/28
// Purpose			:	Virtual functions for GameObject class
// History			:
// 2021/12/01		:	Added messaging
// ---------------------------------------------------------------------------
#include "pch.h"
#include "GameObject.h"

GameObject::GameObject() : comps(std::map<int, Component*>()), isActive(true), isDeletable(false) {}

//will not be called if isActive is set to false
void GameObject::Update()
{
	for(const auto& c : comps)
		c.second->Update();
}

void GameObject::Start()
{
	for (const auto& c : comps)
		c.second->Start();
}

void GameObject::HandleMessage(Message * e)
{
	for (const auto& c : comps)
		c.second->HandleMessage(e);
	delete e;
}


Component* GameObject::getRawComponentPointer(int id)
{
	auto toFind = comps.find(id);
	if (toFind == comps.end())
		return nullptr;
	return comps[id];
}

GameObject::~GameObject()
{
	for (std::map<int, Component*>::iterator current = comps.begin(); current != comps.end(); ++current)
	{
		delete current->second;
	}
	comps.clear();
}

GameObject * GameObject::Clone()
{
	GameObject* toReturn = new GameObject();
	toReturn->tag = tag;
	for (const auto& c : comps)
		toReturn->comps[c.first] = c.second->Clone(toReturn);
	return toReturn;
}

bool GameObject::AllComponentsFinished()
{
	for (const auto& c : comps)
		if (!c.second->isFinishedDeleting)
			return false;
	return true;
}