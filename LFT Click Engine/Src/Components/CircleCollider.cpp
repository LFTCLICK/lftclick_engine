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
#include "Renderer.h"

CircleCollider::CircleCollider()
{
}

void CircleCollider::Start()
{
	Transform* trans = componentOwner->getComponent<Transform>();
	maxBounds = radius;
	componentOwner->colliders.push_back(this);
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
	toReturn->componentOwner = newParent;
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
	if (toCheck != componentOwner)
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
				//g_EventManager->BroadcastMessageToSubscribers(new TriggerCollisionMessage(toCheck->tag));
				//g_EventManager->BroadcastMessageToSubscribers(new CollisionMessage(parent->tag, toCheckPos));

				if (deleteOnCollison) parent->isDeletable = true;
			}*/
			
		}
		else
		{
			toCheckRadius = toCheck->getComponent<CircleCollider>()->radius;
		}

		DirectX::SimpleMath::Vector2 myPos = componentOwner->getComponent<Transform>()->CurrentPos();
		DirectX::SimpleMath::Vector2 toCheckPos = toCheck->getComponent<Transform>()->CurrentPos();

		float distance = DirectX::SimpleMath::Vector2::Distance(myPos, toCheckPos);
		distance = distance - componentOwner->getComponent<CircleCollider>()->radius - toCheckRadius;

		if (distance <= 0)
		{
			componentOwner->HandleMessage(new TriggerCollisionMessage(toCheck->getComponent<CircleCollider>()));
			toCheck->HandleMessage(new TriggerCollisionMessage(componentOwner->getComponent<CircleCollider>()));
			//parent->HandleMessage(new TriggerCollisionMessage(toCheck->tag, toCheck));
			if (deleteOnCollison)
				componentOwner->isActive = false;
		}
	}
}

void CircleCollider::Deserialize(nlohmann::json j, GameObject* componentOwner)
{
	this->componentOwner = componentOwner;
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
			componentOwner->hasNonStaticCollider = true;
	}

	deleteOnCollison = false;
	if (j.contains("deleteOnCollision"))
		deleteOnCollison = j["deleteOnCollison"];
}

void CircleCollider::DebugDraw()
{
	Transform* t = componentOwner->getComponent<Transform>();
	assert(t != nullptr);
	DirectX::SimpleMath::Vector2 debugCirclePos = g_GameManager->mainCamera->WorldToScreenPos(t->CurrentPos(),
		g_Renderer->GetWidth(), g_Renderer->GetHeight());

	g_DebugRenderer->DrawCircle(debugCirclePos, radius, 50.0f);
}
