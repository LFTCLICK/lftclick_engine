// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	EventManager.cpp
// Author			:	Vance Howald
// Creation Date	:	2021/10/28
// Purpose			:	Event Handling 
// History			:
// ---------------------------------------------------------------------------
#include "EventManager.h"

void EventManager::init(GameObjectManager * gom)
{
	this->gom = gom;
}

void EventManager::BroadcastMessage(Message * m)
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

void EventManager::Update()
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

void EventManager::Reset()
{
	messageQueue.clear();
	subscriptions.clear();
}
