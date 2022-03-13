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
#include "SimpleMath.h"

class Collider;

class Message
{
public:
	enum MessageID{COLLISION, TRIGGER_COLLISION};
	MessageID id;
	uint32_t tickTime;
	std::string sourceObjectTag;
	virtual ~Message() {};
	Collider* otherObject;
};

class CollisionMessage : public Message
{
public:
	CollisionMessage(std::string sourceObjectTag, Collider* other, DirectX::SimpleMath::Vector2 deltaPos = { 0,0 }, uint32_t runAtTick = 0) : deltaPos(deltaPos)
	{
		id = MessageID::COLLISION;
		tickTime = runAtTick;
		Message::sourceObjectTag = sourceObjectTag;
		otherObject = other;

	};
	DirectX::SimpleMath::Vector2 deltaPos;
};

class TriggerCollisionMessage : public Message
{
public:
	TriggerCollisionMessage(std::string sourceObjectTag, Collider* other, uint32_t runAtTick = 0)
	{
		id = Message::MessageID::TRIGGER_COLLISION;
		tickTime = runAtTick;
		Message::sourceObjectTag = sourceObjectTag;
		otherObject = other;
	};

};