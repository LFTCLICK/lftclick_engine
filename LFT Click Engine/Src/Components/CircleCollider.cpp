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
#include "DebugRenderer.h"
#include "GameManager.h"
#include "Graphics.h"

extern std::unique_ptr<DebugRenderer> g_debugRenderer;

CircleCollider::CircleCollider()
{
}

void CircleCollider::Start()
{
	Transform* trans = parent->getComponent<Transform>();
	maxBounds = radius;
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
	if (!isStatic)
		newParent->hasNonStaticCollider = true;
	toReturn->deleteOnCollison = deleteOnCollison;
	return (Component*)toReturn;
}

void CircleCollider::CollisionCheck(GameObject* toCheck)
{
	if (toCheck != parent)
	{
		float toCheckRadius = 0;
		if (toCheck->getRawComponentPointer(SQUARE_COLLLIDER))
		{
			toCheckRadius = std::max(toCheck->getComponent<SquareCollider>()->width / 2, toCheck->getComponent<SquareCollider>()->height / 2);
			//too tired to deal with this
			/*DirectX::SimpleMath::Vector2 myPos = parent->getComponent<Transform>()->CurrentPos();
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
				parent->HandleMessage(new TriggerCollisionMessage(toCheck->tag, toCheck));
				toCheck->HandleMessage(new TriggerCollisionMessage(parent->tag, parent));
				//EventManager::getInstance().BroadcastMessageToSubscribers(new TriggerCollisionMessage(toCheck->tag));
				//EventManager::getInstance().BroadcastMessageToSubscribers(new CollisionMessage(parent->tag, toCheckPos));

				if (deleteOnCollison) parent->isDeletable = true;
			}*/
			
		}
		else
		{
			toCheckRadius = toCheck->getComponent<CircleCollider>()->radius;
		}

		DirectX::SimpleMath::Vector2 myPos = parent->getComponent<Transform>()->CurrentPos();
		DirectX::SimpleMath::Vector2 toCheckPos = toCheck->getComponent<Transform>()->CurrentPos();

		float distance = DirectX::SimpleMath::Vector2::Distance(myPos, toCheckPos);
		distance = distance - parent->getComponent<CircleCollider>()->radius - toCheckRadius;

		if (distance <= 0)
		{
			parent->HandleMessage(new TriggerCollisionMessage(toCheck->tag, toCheck->getComponent<CircleCollider>()));
			toCheck->HandleMessage(new TriggerCollisionMessage(parent->tag, parent->getComponent<CircleCollider>()));
			//parent->HandleMessage(new TriggerCollisionMessage(toCheck->tag, toCheck));
			if (deleteOnCollison)
				parent->isActive = false;
		}
	}
}

void CircleCollider::Deserialize(nlohmann::json j, GameObject* parent)
{
	this->parent = parent;
	std::vector<float> centerHelper = j["center"].get<std::vector<float>>();
	center = { centerHelper[0], centerHelper[1] };
	radius = j["radius"];

	isTrigger = false;
	if (j.contains("trigger"))
		isTrigger = j["trigger"];

	isStatic = false;
	if (j.contains("static"))
	{
		isStatic = j["static"];
		if(!isStatic)
			parent->hasNonStaticCollider = true;
	}

	deleteOnCollison = false;
	if (j.contains("deleteOnCollision"))
		deleteOnCollison = j["deleteOnCollison"];
}

void CircleCollider::DebugDraw()
{
	Transform* t = parent->getComponent<Transform>();
	assert(t != nullptr);
	DirectX::SimpleMath::Vector2 debugCirclePos = GameManager::getInstance().mainCamera->WorldToScreenPos(t->CurrentPos(),
		Graphics::getInstance().GetWidth(), Graphics::getInstance().GetHeight());

	g_debugRenderer->DrawCircle(debugCirclePos, radius, 50.0f);
}
