// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	EventManager.cpp
// Author			:	Vance Howald
// Creation Date	:	2021/10/28
// Purpose			:	Event Handling 
// History			:
// ---------------------------------------------------------------------------
#include "pch.h"
#include "EventManager.h"

void EventManager::init(GameObjectManager * gom)
{
	this->gom = gom;//need GameObjectManager for global brodcast
}

void EventManager::BroadcastMessage(Message * m)//sends message to every gameobject
{
	gom->BroadcastMessage(m);
	delete m;
}

void EventManager::BroadcastMessageToSubscribers(Message * m)
{
	std::list<GameObject*>& goList = subscriptions[m->id];
	for (GameObject* g : goList)
	{
		g->HandleMessage(m);
	}
	delete m;
}

void EventManager::AddedTimedMessage(Message * m)
{
	messageQueue.push_back(m);
}

void EventManager::Update()//handles timed messages
{
	std::list<Message*>::iterator it = messageQueue.begin();
	while (it != messageQueue.end())
	{
		if (SDL_GetTicks() >= (*it)->tickTime)
		{
			BroadcastMessageToSubscribers((*it));
			it = messageQueue.erase(it);
		}
		else
			++it;
	}
}

void EventManager::Subscribe(Message::MessageID id, GameObject * g)
{
	subscriptions[id].push_back(g);
}

void EventManager::Unsubscribe(Message::MessageID id, GameObject* g)
{
	subscriptions[id].remove(g);
}

void EventManager::Reset()
{
	messageQueue.clear();
	subscriptions.clear();
}

void EventManager::ProcessCollision()
{
	for (std::unordered_map<Message::MessageID, std::list<GameObject*>>::iterator outerLoop = subscriptions.begin(); outerLoop != subscriptions.end(); outerLoop++)
	{
	
		for (std::list<GameObject*>::iterator innerLoop = outerLoop->second.begin(); innerLoop != outerLoop->second.end(); innerLoop++)
		{
			gom->DoCollision(*innerLoop);
		}
	}
}
