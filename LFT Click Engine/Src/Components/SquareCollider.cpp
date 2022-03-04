// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	SquareCollider.cpp
// Author			:	Vance Howald
// Creation Date	:	2021/11/14
// Purpose			:	Square collider handling
// History			:
// ---------------------------------------------------------------------------
#include "pch.h"
#include "SquareCollider.h"
#include"Transform.h"
#include "DebugRenderer.h"
#include "GameManager.h"
#include "Graphics.h"
#include "GameManager.h"

using namespace DirectX::SimpleMath;

extern std::unique_ptr<DebugRenderer> g_debugRenderer;

SquareCollider::SquareCollider()
{
}

void SquareCollider::Start()
{
	Transform* trans = parent->getComponent<Transform>();
	center.x *= trans->scale.x;
	center.y *= trans->scale.y;
	points = new float[8];
	points[0] = (.5f*width) + center.x;
	points[1] = (.5f*height) + center.y;
	points[2] = (.5f*width) + center.x;
	points[3] = (-.5f*height) + center.y;
	points[4] = (-.5f*width) + center.x;
	points[5] = (-.5f*height) + center.y;
	points[6] = (-.5f*width) + center.x;
	points[7] = (.5f*height) + center.y;
}

void SquareCollider::Update()
{
}

int SquareCollider::getCompId()
{
	return ComponentType::SQUARE_COLLLIDER;
}

Component * SquareCollider::Clone(GameObject * newParent)
{
	SquareCollider* toReturn = new SquareCollider();
	toReturn->parent = newParent;
	toReturn->center = center;
	toReturn->width = width;
	toReturn->height = height;
	toReturn->isTrigger = isTrigger;
	toReturn->isStatic = isStatic;
	toReturn->deleteOnCollison = deleteOnCollison;
	return (Component*) toReturn;
}

void SquareCollider::CollisionCheck(GameObject* toCheck)
{
	if (toCheck != parent)
	{
		if (toCheck->getRawComponentPointer(13))
		{
			return;
		}
		else
		{
			SquareCollider* toCheckCollider = toCheck->getComponent<SquareCollider>();
			DirectX::XMVECTOR myPos = DirectX::XMVectorAdd(parent->getComponent<Transform>()->GetPosXMVector(), DirectX::XMLoadFloat4(&center));
			DirectX::XMVECTOR toCheckPos = DirectX::XMVectorAdd(toCheck->getComponent<Transform>()->GetPosXMVector(), DirectX::XMLoadFloat4(&toCheckCollider->center));

			DirectX::XMFLOAT2 myPosf;
			DirectX::XMStoreFloat2(&myPosf, myPos);

			DirectX::XMFLOAT2 toCheckPosf;
			DirectX::XMStoreFloat2(&toCheckPosf, toCheckPos);

			DirectX::SimpleMath::Rectangle a = DirectX::SimpleMath::Rectangle(myPosf.x - (width / 2), myPosf.y - (height / 2), width, height);
			DirectX::SimpleMath::Rectangle b = DirectX::SimpleMath::Rectangle(toCheckPosf.x - (toCheckCollider->width / 2), toCheckPosf.y - (toCheckCollider->height / 2),
				toCheckCollider->width, toCheckCollider->height);

			bool intersects = a.Intersects(b);

			if (intersects)
			{
				if (isTrigger)
				{
					toCheck->HandleMessage(new DamageCollisionMessage(parent->tag, parent));
				}
				else
				{
					DirectX::SimpleMath::Vector2 delta;
					DirectX::XMVECTOR difference = DirectX::XMVectorSubtract(myPos, toCheckPos);
					difference.m128_f32[0] = fabs(difference.m128_f32[0]) - ((width + toCheckCollider->width) / 2);
					difference.m128_f32[1] = fabs(difference.m128_f32[1]) - ((height + toCheckCollider->height) / 2);
					if (difference.m128_f32[1] > difference.m128_f32[0])
					{
						if (myPos.m128_f32[1] < toCheckPos.m128_f32[1])
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
						if (myPos.m128_f32[0] < toCheckPos.m128_f32[0])
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
					toCheck->HandleMessage(new CollisionMessage(parent->tag, parent, delta));
				}
				if (deleteOnCollison) parent->isDeletable = true;
			}
		}
	}
}

float * SquareCollider::getPoints()
{
	return points;
}

void SquareCollider::Deserialize(nlohmann::json j, GameObject* parent)
{
	this->parent = parent;
	std::vector<float> centerHelper = j["center"].get<std::vector<float>>();
	center = { centerHelper[0], centerHelper[1], centerHelper[2], 0 };
	width = j["width"];
	height = j["height"];
	isTrigger = j["trigger"];
	//isStatic = j["static"];
	deleteOnCollison = j["deleteOnCollison"];
}

void SquareCollider::DebugDraw()
{
	Transform* t = parent->getComponent<Transform>();
	assert(t != nullptr);

	auto cam = GameManager::getInstance().mainCamera;
	float screenWidth = Graphics::getInstance().GetWidth();
	float screenHeight = Graphics::getInstance().GetHeight();

	Vector2 a = t->CurrentPos() - Vector2(width / 2.0f, -height / 2.0f);
	Vector2 b = t->CurrentPos() + Vector2(width / 2.0f, height / 2.0f);
	Vector2 c = t->CurrentPos() + Vector2(width / 2.0f, -height / 2.0f);
	Vector2 d = t->CurrentPos() - Vector2(width / 2.0f, height / 2.0f);

	a = cam->WorldToScreenPos(a, screenWidth, screenHeight);
	b = cam->WorldToScreenPos(b, screenWidth, screenHeight);
	c = cam->WorldToScreenPos(c, screenWidth, screenHeight);
	d = cam->WorldToScreenPos(d, screenWidth, screenHeight);

	g_debugRenderer->DrawQuad(a, b, c, d);
}
