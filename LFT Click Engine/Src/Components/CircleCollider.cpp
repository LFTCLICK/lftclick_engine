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
}

void CircleCollider::Update()
{
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
	DirectX::SimpleMath::Vector2 debugCirclePos = g_GameManager->mainCamera->WorldToScreenPos(t->CurrentPos()+center,
		g_Renderer->GetWidth(), g_Renderer->GetHeight());

	g_DebugRenderer->DrawCircle(debugCirclePos, radius, 50.0f);
}
