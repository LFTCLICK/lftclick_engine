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
#include <map>

class GameObject
{
public:
	GameObject();
	void Update();
	void Start();
	template <typename T>
	void Add(T* c)
	{
		comps[c->getCompId()] = (Component*)c;
	}
	template <typename T>
	void Remove(T* c)
	{
		comps.erase(c->getCompId());
	}
	void HandleMessage(Message* e);
	template <typename T>
	T* getComponent()
	{
		T temp;
		auto toFind = comps.find(temp.getCompId());
		if (toFind == comps.end())
			return nullptr;
		return (T*)comps[temp.getCompId()];
	}
	~GameObject();
public:
	std::string tag;
	bool isActive;
private:
	std::map<int, Component*> comps;
	GameObject* Clone();//ONLY FACTORY SHOULD TOUCH THIS
	friend class GameObjectFactory;
};