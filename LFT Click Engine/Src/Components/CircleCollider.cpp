// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	CircleCollider.cpp
// Author			:	Rohit Punjabi
// Creation Date	:	2022/02/25
// Purpose			:	Circle collider handling
// History			:
// ---------------------------------------------------------------------------

#include "pch.h"
#include "CircleCollider.h"
#include "Squarecollider.h"
#include"Transform.h"

CircleCollider::CircleCollider()
{
}

void CircleCollider::Start()
{
	Transform* trans = parent->getComponent<Transform>();
	//center.x *= trans->scale.x;
	//center.y *= trans->scale.y;
}

void CircleCollider::Update()
{
}

int CircleCollider::getCompId()
{
	return ComponentType::CIRCLE_COLLIDER;
}

Component* CircleCollider::Clone(GameObject* newParent)
{
	CircleCollider* toReturn = new CircleCollider();
	toReturn->parent = newParent;
	toReturn->center = center;
	toReturn->radius = radius;
	toReturn->isTrigger = isTrigger;
	toReturn->isStatic = isStatic;
	toReturn->deleteOnCollison = deleteOnCollison;
	return (Component*)toReturn;
}

void CircleCollider::CollisionCheck(GameObject* toCheck)
{
	if (toCheck != parent)
	{
		if (toCheck->getRawComponentPointer(SQUARE_COLLLIDER))
		{
			DirectX::SimpleMath::Vector2 myPos = parent->getComponent<Transform>()->CurrentPos();
			DirectX::SimpleMath::Vector2 toCheckPos = toCheck->getComponent<Transform>()->CurrentPos();

			DirectX::SimpleMath::Vector2 circleDistance;

			circleDistance.x = abs(myPos.x - toCheckPos.x);
			circleDistance.y = abs(myPos.y - toCheckPos.y);

			if (circleDistance.x > (toCheck->getComponent<SquareCollider>()->width / 2) + parent->getComponent<CircleCollider>()->radius
				|| circleDistance.y > (toCheck->getComponent<SquareCollider>()->height / 2) + parent->getComponent<CircleCollider>()->radius)
			{
				return;
			}

			if (circleDistance.x <= toCheck->getComponent<SquareCollider>()->width / 2
				|| circleDistance.y <= toCheck->getComponent<SquareCollider>()->height / 2)
			{
				parent->HandleMessage(new DamageCollisionMessage(toCheck->tag, toCheck));
				toCheck->HandleMessage(new DamageCollisionMessage(parent->tag, parent));
				//EventManager::getInstance().BroadcastMessageToSubscribers(new DamageCollisionMessage(toCheck->tag));
				//EventManager::getInstance().BroadcastMessageToSubscribers(new CollisionMessage(parent->tag, toCheckPos));

				if (deleteOnCollison) parent->isDeletable = true;
			}
			
		}
		else
		{
			CircleCollider* toCheckCollider = toCheck->getComponent<CircleCollider>();

			DirectX::SimpleMath::Vector2 myPos = parent->getComponent<Transform>()->CurrentPos();
			DirectX::SimpleMath::Vector2 toCheckPos = toCheck->getComponent<Transform>()->CurrentPos();

			float distance = DirectX::SimpleMath::Vector2::Distance(myPos, toCheckPos);
			distance = distance - parent->getComponent<CircleCollider>()->radius - toCheck->getComponent<CircleCollider>()->radius;

			if (distance <= 0)
			{
				parent->HandleMessage(new DamageCollisionMessage(toCheck->tag, toCheck));
				toCheck->HandleMessage(new DamageCollisionMessage(parent->tag, parent));
				//parent->HandleMessage(new DamageCollisionMessage(toCheck->tag, toCheck));
				if (deleteOnCollison)
					parent->isActive = false;
			}
		}
	}
}

void CircleCollider::Deserialize(nlohmann::json j, GameObject* parent)
{
	this->parent = parent;
	std::vector<float> centerHelper = j["center"].get<std::vector<float>>();
	center = { centerHelper[0], centerHelper[1], centerHelper[2], 0 };
	radius = j["radius"];
	isTrigger = j["trigger"];
	isStatic = j["static"];
	deleteOnCollison = j["deleteOnCollison"];
}
