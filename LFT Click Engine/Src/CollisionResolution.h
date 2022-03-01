// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	CollisionResoultion.h
// Author			:	Vance Howald
// Creation Date	:	2021/03/02
// Purpose			:	Gets vec2 for collisions
// History			: 
// ---------------------------------------------------------------------------
#pragma once
#include "pch.h"
#include "./Components/Component.h"
#include "./Components/Transform.h"
#include "./Components/Collider.h"
#include "./Components/SquareCollider.h"
#include "./Components/CircleCollider.h"

class CollisionResolution{
private:
	static DirectX::SimpleMath::Vector2 SquareSquareCollision(Collider* a, DirectX::SimpleMath::Vector2 aPos, Collider* b, DirectX::SimpleMath::Vector2 bPos, bool& didCollide)
	{
		SquareCollider* aRealCollider = (SquareCollider*)a;
		SquareCollider* toCheckCollider = (SquareCollider*)b;

		DirectX::SimpleMath::Rectangle aRect = DirectX::SimpleMath::Rectangle(aPos.x - (aRealCollider->width / 2), aPos.y - (aRealCollider->height / 2), aRealCollider->width, aRealCollider->height);
		DirectX::SimpleMath::Rectangle bRect = DirectX::SimpleMath::Rectangle(bPos.x - (toCheckCollider->width / 2), bPos.y - (toCheckCollider->height / 2),
			toCheckCollider->width, toCheckCollider->height);

		didCollide = aRect.Intersects(bRect);

		if (didCollide)
		{
			if (a->isTrigger||b->isTrigger)
			{
				return DirectX::SimpleMath::Vector2();
			}
			else
			{
				DirectX::SimpleMath::Vector2 delta;
				DirectX::XMVECTOR difference = DirectX::XMVectorSubtract(aPos, bPos);
				difference.m128_f32[0] = fabs(difference.m128_f32[0]) - ((aRealCollider->width + toCheckCollider->width) / 2);
				difference.m128_f32[1] = fabs(difference.m128_f32[1]) - ((aRealCollider->height + toCheckCollider->height) / 2);
				if (difference.m128_f32[1] > difference.m128_f32[0])
				{
					if (aPos.y < bPos.y)
					{
						delta = DirectX::SimpleMath::Vector2(0.0f, -difference.m128_f32[1]);
					}
					else
					{
						delta = DirectX::SimpleMath::Vector2(0.0f, difference.m128_f32[1]);
					}
				}
				else
				{
					if (aPos.x < bPos.x)
					{
						delta = DirectX::SimpleMath::Vector2(-difference.m128_f32[0], 0);
					}
					else
					{
						delta = DirectX::SimpleMath::Vector2(difference.m128_f32[0], 0);
					}
				}
				if (difference.m128_f32[1] > 0.1 || difference.m128_f32[1] < 0.1)
				{
					int a = 0;
				}
				return delta;
			}
			//if (deleteOnCollison) parent->isDeletable = true;
		}
		return DirectX::SimpleMath::Vector2();
	}
	static DirectX::SimpleMath::Vector2 CircleCircleCollision(Collider* a, DirectX::SimpleMath::Vector2 aPos, Collider* b, DirectX::SimpleMath::Vector2 bPos, bool& didCollide)
	{
		CircleCollider* aRealCollider = (CircleCollider*)a;
		CircleCollider* toCheckCollider = (CircleCollider*)b;
		float distance = DirectX::SimpleMath::Vector2::Distance(aPos, bPos);
		distance = distance - aRealCollider->radius - toCheckCollider->radius;

		if (distance <= 0.0001)
			didCollide = true;
		else
			didCollide = false;
		return DirectX::SimpleMath::Vector2();
	}
	static DirectX::SimpleMath::Vector2 CircleSquareCollision(Collider* a, DirectX::SimpleMath::Vector2 aPos, Collider* b, DirectX::SimpleMath::Vector2 bPos, bool& didCollide)
	{
		CircleCollider* aRealCollider = (CircleCollider*)a;
		SquareCollider* toCheckCollider = (SquareCollider*)b;

		DirectX::SimpleMath::Vector2 circleDistance;

		circleDistance.x = abs(aPos.x - bPos.x);
		circleDistance.y = abs(aPos.y - bPos.y);

		if (circleDistance.x > (toCheckCollider->width / 2) + aRealCollider->radius
			|| circleDistance.y > (toCheckCollider->height / 2) + aRealCollider->radius)
		{
			didCollide = false;
		}
		else if (circleDistance.x <= toCheckCollider->width / 2
			|| circleDistance.y <= toCheckCollider->height / 2)
		{
			didCollide = true;
		}
		else
			didCollide = false;
		return DirectX::SimpleMath::Vector2();
	}

public:

	static DirectX::SimpleMath::Vector2 ProcessCollision(Collider* a, DirectX::SimpleMath::Vector2 aPos, Collider* b, DirectX::SimpleMath::Vector2 bPos, bool &didCollide)
	{

		if (a->isTrigger || b->isTrigger)
			return DirectX::SimpleMath::Vector2();
		if (a->getCompId() == Component::ComponentType::SQUARE_COLLLIDER && b->getCompId() == Component::ComponentType::SQUARE_COLLLIDER)
		{
			return SquareSquareCollision(a, aPos, b, bPos, didCollide);
		}
		else if (a->getCompId() == Component::ComponentType::CIRCLE_COLLIDER && b->getCompId() == Component::ComponentType::CIRCLE_COLLIDER)
		{
			return CircleCircleCollision(a, aPos, b, bPos, didCollide);
		}
		else if ((a->getCompId() == Component::ComponentType::SQUARE_COLLLIDER || b->getCompId() == Component::ComponentType::SQUARE_COLLLIDER) && (a->getCompId() == Component::ComponentType::CIRCLE_COLLIDER || b->getCompId() == Component::ComponentType::CIRCLE_COLLIDER))
		{
			if(a->getCompId() == Component::ComponentType::SQUARE_COLLLIDER)
				return -CircleSquareCollision(a, aPos, b, bPos, didCollide);
			else
			{
				return CircleSquareCollision(b, bPos, a, aPos, didCollide);
			}
		}
		return DirectX::SimpleMath::Vector2();
	}

};