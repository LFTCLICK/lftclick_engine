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
#include "Components\Drawable.h"
#include "Components\Collider.h"
#include "Components\SquareCollider.h"
#include "Components\CircleCollider.h"
#include "CollisionResolution.h"
#include "GameManager.h"

const int COLLIDER_IDS[] = { Component::SQUARE_COLLLIDER, Component::CIRCLE_COLLIDER };
const int NUMBER_OF_COLLIDERS = 2;

GameObjectManager::GameObjectManager()
{
	gameObjectList = std::list<GameObject*>();
}

void GameObjectManager::Update()
{
	//std::cout << "Size: " << gameObjectList.size() << std::endl;
	std::list<GameObject*>::iterator objIt = gameObjectList.begin();

	while (objIt != gameObjectList.end()) {
		GameObject* g = *objIt;

		if (g->isDeletable) {
			EventManager::getInstance().UnsubscribeFromAllEvents(g);
			objIt = gameObjectList.erase(objIt);
			delete g;
		}
		else {
			if (g->isActive) g->Update();
			++objIt;
		}
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

	static bool debugDraw = false;
	ImGui::Checkbox("Draw Colliders", &debugDraw);

	for (GameObject* g : gameObjectList)
	{
		if (g->isActive)
		{
			Drawable* s = g->getComponent<Drawable>();
			if (s != nullptr)
			{
				s->Draw();
			}

			if (!debugDraw)
				continue;

			CircleCollider* c = g->getComponent<CircleCollider>();
			
			if (c != nullptr)
			{
				c->DebugDraw();
			}

			SquareCollider* sq = g->getComponent<SquareCollider>();

			if (sq != nullptr)
			{
				sq->DebugDraw();
			}
		}
	}
}

void GameObjectManager::ProcessCollision()
{
	for (auto mainListItt = gameObjectList.begin(); mainListItt != gameObjectList.end(); ++mainListItt)
	{
		if (!(*mainListItt)->isActive || (*mainListItt)->isDeletable)
			continue;
		if (!(*mainListItt)->hasNonStaticCollider)
			break;
		Transform* mainTrans = (*mainListItt)->getComponent<Transform>();
		for (int mainColliderIndex=0; mainColliderIndex<NUMBER_OF_COLLIDERS; ++mainColliderIndex)
		{
			Collider* s = (Collider* )(*mainListItt)->getRawComponentPointer(COLLIDER_IDS[mainColliderIndex]);
			if (s == nullptr)
				continue;
			for (auto innerLoop = std::next(mainListItt); innerLoop != gameObjectList.end(); ++innerLoop)
			{
				if (!(*innerLoop)->isActive || (*innerLoop)->isDeletable)
					continue;
				Transform* toCheckTrans = (*innerLoop)->getComponent<Transform>();
				for (int innerColliderIndex = 0; innerColliderIndex < NUMBER_OF_COLLIDERS; ++innerColliderIndex)
				{
					Collider* toCheckWith = (Collider*)(*innerLoop)->getRawComponentPointer(COLLIDER_IDS[innerColliderIndex]);
					if (toCheckWith == nullptr)
						continue;
					//if (toCheckWith->isStatic && s->isStatic) //this should never happen now
					//	continue;
					bool didCollide = false;
					DirectX::SimpleMath::Vector2 deltaPos = CollisionResolution::ProcessCollision(s, mainTrans->position, toCheckWith, toCheckTrans->position, didCollide);
					if (didCollide)
					{
						if (s->isTrigger || toCheckWith->isTrigger)
						{
							(*mainListItt)->HandleMessage(new TriggerCollisionMessage((*innerLoop)->tag, toCheckWith));
							(*innerLoop)->HandleMessage(new TriggerCollisionMessage((*mainListItt)->tag, s));
						}
						else
						{
							if (!s->isStatic && !toCheckWith->isStatic)
								deltaPos *= .5f;
							(*mainListItt)->HandleMessage(new CollisionMessage((*innerLoop)->tag, toCheckWith, -deltaPos));
							(*innerLoop)->HandleMessage(new CollisionMessage((*mainListItt)->tag, s, deltaPos));
						}
					}
				}
			}
		}
	}
}

void GameObjectManager::DoCollision(GameObject* toCheckWith)
{
	/*for (std::list<GameObject*>::iterator it = gameObjectList.begin(); it != gameObjectList.end();)
	{
		GameObject* toCheck = *it;
		++it;
		for (std::list<GameObject*>::iterator innerIt = it; innerIt != gameObjectList.end(); ++innerIt)
		{
			CollisionResolution
		}
	}*/
	for (GameObject* g : gameObjectList)
	{
		if (g->isActive && !g->isDeletable && g != toCheckWith)
		{
			Collider* s = dynamic_cast<Collider*>(g->getComponent<SquareCollider>());
			if (s != nullptr)
				s->CollisionCheck(toCheckWith);
			s = dynamic_cast<Collider*>(g->getComponent<CircleCollider>());
			if (s != nullptr) 
				s->CollisionCheck(toCheckWith);
		}
	}
}

void GameObjectManager::Deserialize(GameObjectFactory * gof, json j, bool isPrefab)
{
	this->gof = gof;
	json prefabsJSON = j["Prefabs"];
	for (json::iterator prefabs = prefabsJSON.begin(); prefabs != prefabsJSON.end(); ++prefabs)
	{
		GameObject* returned = gof->CreateObject(prefabs.value());
		returned->isActive = false;
		prefabList.push_back(returned);
	}
	prefabsJSON = j["Level"];
	for (json::iterator currentObj = prefabsJSON.begin(); currentObj != prefabsJSON.end(); ++currentObj)
	{
		GameObject* newOne = ClonePrefabOfTag(gof, currentObj.value()["object"], true);
		std::string newObjectTag = currentObj.value()["object"];
		json overrideList = currentObj.value()["overrides"];
		for (json::iterator c = overrideList.begin(); c != overrideList.end(); ++c)
		{
			json actualOverride = c.value();
			for (json::iterator realOverrides = actualOverride.begin(); realOverrides != actualOverride.end(); ++realOverrides)
			{
				int val = std::stoi(realOverrides.key());
				newOne->getRawComponentPointer(std::stoi(realOverrides.key()))->Deserialize(realOverrides.value(), newOne);
			}
		}
		//newOne->Start();
	}
	for (GameObject* g : gameObjectList)
		g->Start();
}

void GameObjectManager::AddGameObject(GameObject * go)
{
	if(go->hasNonStaticCollider)
		gameObjectList.push_front(go);
	else
		gameObjectList.push_back(go);
}

void GameObjectManager::DeleteAll()
{
	for (GameObject* g : gameObjectList)
		delete g;
	for (GameObject* g : prefabList)
		delete g;
	gameObjectList.clear();
	prefabList.clear();
}

void GameObjectManager::DeleteObjectOfTag(std::string tag) 
{
	std::list<GameObject*>::iterator i = gameObjectList.begin();
	while (i != gameObjectList.end()) {
		if ((*i)->tag == tag)
			gameObjectList.erase(i++);  // alternatively, i = items.erase(i);
		else
			++i;
	}
}

GameObject * GameObjectManager::ClonePrefabOfTag(GameObjectFactory * gof, std::string tag, bool skipStart)
{
	for (GameObject* g : prefabList)
	{
		if (g->tag == tag)
		{
			GameObject* go = gof->CloneObject(g);
			if(!skipStart)
				go->Start();
			AddGameObject(go);
			return go;
		}
	}
	return nullptr;
}

GameObject * GameObjectManager::CloneObject(GameObject * go)
{
	GameObject* toReturn = gof->CloneObject(go);
	toReturn->tag = go->tag;
	AddGameObject(toReturn);
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
	DeleteAll();

	gameObjectList.clear();
}
