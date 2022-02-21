// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	EventManager.h
// Author			:	Vance Howald
// Creation Date	:	2021/10/28
// Purpose			:	Event Handling 
// History			:
// ---------------------------------------------------------------------------

#pragma once

#include "GameObjectManager.h"
#include "Components/GameObject.h"
#include <list>
#include "Messages.h"
#include <SDL.h>

class EventManager
{
public:

	static EventManager& getInstance()
	{
		static EventManager instance;
		return instance;
	}
	void init(GameObjectManager* gom);
	void BroadcastMessage(Message* m);
	void BroadcastMessageToSubscribers(Message* m);
	void AddedTimedMessage(Message* m);
	void Update();
	void Subscribe(Message::MessageID id, GameObject* g);
	void Unsubscribe(Message::MessageID id, GameObject* g);
	bool RemoveOld(const Message& m) { return SDL_GetTicks() >= m.tickTime; }
	void Reset();
	void ProcessCollision();

private:
	GameObjectManager* gom;
	std::list<Message*> messageQueue;
	std::unordered_map<Message::MessageID, std::list<GameObject*>> subscriptions;
};