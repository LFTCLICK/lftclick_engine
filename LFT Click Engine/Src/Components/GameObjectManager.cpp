// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	GameObjectManager.cpp
// Author			:	Vance Howald
// Creation Date	:	2021/10/28
// Purpose			:	Manages GameObjects
// History			:
// 2021/12/01		-	Added collider handeling and message sending
// ---------------------------------------------------------------------------
#include "pch.h"
#include "GameObjectManager.h"
#include "Drawable.h"
#include "Collider.h"
#include "MeshCollider.h"
#include "SquareCollider.h"

GameObjectManager::GameObjectManager()
{
	gameObjectList = std::list<GameObject*>();
}

void GameObjectManager::Update()
{
	for (GameObject* g : gameObjectList)
	{
		if (g->isActive)
			g->Update();
	}
}

void GameObjectManager::Start()
{
	//note that even if the object is inactive start is called
	for (GameObject* g : gameObjectList)
		g->Start();
}

void GameObjectManager::Draw()
{
	for (GameObject* g : gameObjectList)
	{
		if (g->isActive)
		{
			Drawable* s = g->getComponent<Drawable>();
			if (s != nullptr)
			{
				s->Draw();
			}
		}
	}
}

void GameObjectManager::DoCollision(GameObject* toCheckWith)
{
	for (GameObject* g : gameObjectList)
	{
		if (g->isActive && g!=toCheckWith)
		{
			Collider* s = dynamic_cast<Collider*>(g->getComponent<MeshCollider>());
			if (s == nullptr)
				s = dynamic_cast<Collider*>(g->getComponent<SquareCollider>());
			if (s != nullptr)
			{
				s->CollisionCheck(toCheckWith);
			}
		}
	}
}

void GameObjectManager::Deserialize(GameObjectFactory * gof, json j, bool isPrefab)
{
	this->gof = gof;
	for (json::iterator level = j.begin(); level != j.end(); ++level)
	{
		for (json::iterator currentObj = level.value().begin(); currentObj != level.value().end(); ++currentObj)
		{
			GameObject* returned = gof->CreateObject(currentObj.value());
			if (isPrefab)//make it inactive if it's a prefab
				returned->isActive = false;
			gameObjectList.push_back(returned);
		}
	}
}

void GameObjectManager::AddGameObject(GameObject * go)
{
	gameObjectList.push_back(go);
}

void GameObjectManager::DeleteAll()
{
	for (GameObject* g : gameObjectList)
		delete g;
	gameObjectList.clear();
}

GameObject * GameObjectManager::CloneObjectOfTag(GameObjectFactory * gof, std::string tag)
{
	for (GameObject* g : gameObjectList)
	{
		if (g->tag == tag)
		{
			GameObject* go = gof->CloneObject(g);
			gameObjectList.push_back(go);
			return go;
		}
	}
	return nullptr;
}

GameObject * GameObjectManager::CloneObject(GameObject * go)
{
	GameObject* toReturn = gof->CloneObject(go);
	toReturn->tag = go->tag;
	gameObjectList.push_back(toReturn);
	toReturn->Start();
	return toReturn;

}

GameObject * GameObjectManager::FindObjectOfTag(std::string tag)
{
	for (GameObject* g : gameObjectList)
	{
		if (g->tag == tag)
		{
			return g;
		}
	}
	return nullptr;
}

void GameObjectManager::BroadcastMessage(Message * m)
{
	for (GameObject* g : gameObjectList)
	{
		g->HandleMessage(m);
	}
}

GameObjectManager::~GameObjectManager()
{
	gameObjectList.clear();
}
