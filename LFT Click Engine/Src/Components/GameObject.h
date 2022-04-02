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

class Transform;
class Collider;

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
		auto toFind = comps.find(T::getStaticCompId());
		if (toFind == comps.end()) {
			return nullptr;
		}
		return (T*)(toFind->second);
	}
	Component* getRawComponentPointer(int id);
	~GameObject();
public:
	std::string tag;
	bool isActive;
	bool hasNonStaticCollider;
	bool isDeletable;
	std::vector<Collider*> colliders;
	Transform* trans;
	bool isOnScreen;
	std::map<int, Component*> comps;
private:
	GameObject* Clone();//ONLY FACTORY SHOULD TOUCH THIS
	friend class GameObjectFactory;
};