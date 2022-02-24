// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	GameObjectManager.h
// Author			:	Vance Howald
// Creation Date	:	2021/10/28
// Purpose			:	Manages GameObjects
// History			:
// 2021/12/01		-	Added collider handeling and message sending
// ---------------------------------------------------------------------------
#pragma once

#include <list>
#include <json.hpp>
#include "Components\GameObject.h"
#include "GameObjectFactory.h"
#include <SDL.h>

using json = nlohmann::json;

class GameObjectManager
{
public:
	static GameObjectManager& getInstance()
	{
		static GameObjectManager instance;
		return instance;
	}
	void Update();
	void Start();
	void Draw();
	void DoCollision(GameObject * toCheckWith);
	void Deserialize(GameObjectFactory * gof, json j, bool isPrefab = false);
	void AddGameObject(GameObject* go);
	void DeleteAll();
	GameObject* CloneObjectOfTag(GameObjectFactory * gof, std::string tag);
	GameObject* CloneObject(GameObject* go);
	GameObject* FindObjectOfTag(std::string tag);
	void BroadcastMessage(Message* m);
	~GameObjectManager();

private:
	GameObjectManager();
	GameObjectFactory * gof;
	std::list<GameObject*> gameObjectList;
};