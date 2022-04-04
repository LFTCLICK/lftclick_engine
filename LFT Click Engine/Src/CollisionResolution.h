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
		
		aPos += aRealCollider->center;
		bPos += toCheckCollider->center;

		DirectX::SimpleMath::Rectangle aRect = DirectX::SimpleMath::Rectangle(aPos.x - (aRealCollider->clientWidth / 2), aPos.y - (aRealCollider->clientHeight / 2), aRealCollider->clientWidth, aRealCollider->clientHeight);
		DirectX::SimpleMath::Rectangle bRect = DirectX::SimpleMath::Rectangle(bPos.x - (toCheckCollider->clientWidth / 2), bPos.y - (toCheckCollider->clientHeight / 2),
			toCheckCollider->clientWidth, toCheckCollider->clientHeight);

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
				difference.m128_f32[0] = fabs(difference.m128_f32[0]) - ((aRealCollider->clientWidth + toCheckCollider->clientWidth) / 2);
				difference.m128_f32[1] = fabs(difference.m128_f32[1]) - ((aRealCollider->clientHeight + toCheckCollider->clientHeight) / 2);
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
		aPos += aRealCollider->center;
		bPos += toCheckCollider->center;
		float distance = DirectX::SimpleMath::Vector2::Distance(aPos, bPos);
		distance = distance - aRealCollider->radius - toCheckCollider->radius;

		if (distance <= 0.0001)
			didCollide = true;
		else
			didCollide = false;
		if (didCollide)
		{
			if (a->isTrigger || b->isTrigger)
				return DirectX::SimpleMath::Vector2();

			DirectX::SimpleMath::Vector2 delta =  bPos- aPos;
			delta = -distance / (DirectX::SimpleMath::Vector2::Distance(DirectX::SimpleMath::Vector2(0, 0), delta)) * delta;
			return (delta);
		}
		return DirectX::SimpleMath::Vector2();
	}

	static DirectX::SimpleMath::Vector2 CircleSquareCollision(Collider* a, DirectX::SimpleMath::Vector2 aPos, Collider* b, DirectX::SimpleMath::Vector2 bPos, bool& didCollide)
	{
		CircleCollider* aRealCollider = (CircleCollider*)a;
		SquareCollider* toCheckCollider = (SquareCollider*)b;
		aPos += aRealCollider->center;
		bPos += toCheckCollider->center;
		DirectX::SimpleMath::Vector2 intersectionPoint = DirectX::SimpleMath::Vector2(
			std::max(bPos.x - (toCheckCollider->clientWidth / 2), std::min(aPos.x, bPos.x + (toCheckCollider->clientWidth / 2))),
			std::max(bPos.y - (toCheckCollider->clientHeight / 2), std::min(aPos.y, bPos.y + (toCheckCollider->clientHeight / 2))));

		float distance = DirectX::SimpleMath::Vector2::Distance(aPos, intersectionPoint);
		distance = distance - aRealCollider->radius;
		if (distance <= 0.0001)
			didCollide = true;
		else
			didCollide = false;
		if (didCollide)
		{
			if (a->isTrigger || b->isTrigger)
				return DirectX::SimpleMath::Vector2();

			DirectX::SimpleMath::Vector2 delta = aPos-intersectionPoint;
			delta = -distance / (DirectX::SimpleMath::Vector2::Distance(DirectX::SimpleMath::Vector2(0, 0), delta)) * delta;
			if (DirectX::SimpleMath::Vector2::Distance(DirectX::SimpleMath::Vector2(0, 0), delta) > 200)
			{
				int a = 0;
			}
			return (delta);
		}
		return DirectX::SimpleMath::Vector2();

	}

public:

	static DirectX::SimpleMath::Vector2 ProcessCollision(Collider* a, DirectX::SimpleMath::Vector2 aPos, Collider* b, DirectX::SimpleMath::Vector2 bPos, bool &didCollide)
	{
		float maxBoundsSquared = a->maxBounds + b->maxBounds;
		float xDif = aPos.x - bPos.x;
		float yDif = aPos.y - bPos.y;
		if ((xDif * xDif) + (yDif * yDif) > maxBoundsSquared * maxBoundsSquared)//Vector2::DistanceSquared converts both vectors to XMVECTOR and then does math on them, this is about 3 times faster
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
			if(a->getCompId() == Component::ComponentType::CIRCLE_COLLIDER)
				return -CircleSquareCollision(a, aPos, b, bPos, didCollide);
			else
				return CircleSquareCollision(b, bPos, a, aPos, didCollide);
		}
		return DirectX::SimpleMath::Vector2();
	}

};