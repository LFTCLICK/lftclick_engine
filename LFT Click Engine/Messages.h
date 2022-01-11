// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	Messages.h
// Author			:	Vance Howald
// Creation Date	:	2021/12/01
// Purpose			:	Message objects
// History			:
// ---------------------------------------------------------------------------
#pragma once
#include <string>
#include "Vector2D.h"

class Message
{
public:
	enum MessageID{COLLISION, DAMAGE_COLLISION, SLOW_PICKUP, INVIN_PICKUP};
	MessageID id;
	uint32_t tickTime;
	std::string sourceObjectTag;
	virtual ~Message() {};
};

class CollisionMessage : public Message
{
public:
	CollisionMessage(std::string sourceObjectTag, Vector2D deltaPos = { 0,0 }, uint32_t runAtTick = 0) : deltaPos(deltaPos)
	{
		id = Message::MessageID::COLLISION;
		tickTime = runAtTick;
		Message::sourceObjectTag = sourceObjectTag;

	};
	Vector2D deltaPos;
};

class DamageCollisionMessage : public Message
{
public:
	DamageCollisionMessage(std::string sourceObjectTag, uint32_t runAtTick = 0)
	{
		id = Message::MessageID::DAMAGE_COLLISION;
		tickTime = runAtTick;
		Message::sourceObjectTag = sourceObjectTag;
	};

};