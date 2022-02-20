// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	GameObjectFactory.h
// Author			:	Vance Howald
// Creation Date	:	2021/10/28
// Purpose			:	Header
// History			:
// ---------------------------------------------------------------------------
#pragma once
#include <string>
#include "Components/GameObject.h"
#include <json.hpp>
using json = nlohmann::json;

class GameObjectFactory
{
public:
	GameObjectFactory();
	GameObject* const CreateObject(json type);
	GameObject* const CloneObject(GameObject* toClone);

private:
	Component* const CreateComp(json::iterator type, GameObject* currentObj);
	friend class GameObject;
};