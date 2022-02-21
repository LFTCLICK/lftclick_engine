// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	MessCollider.cpp
// Author			:	Vance Howald
// Creation Date	:	2021/10/28
// Purpose			:	Mesh collision handling
// ---------------------------------------------------------------------------

#include "pch.h"
#include "MeshCollider.h"
#include "SquareCollider.h"
#include "Math2D.h"

MeshCollider::MeshCollider()
{
}

MeshCollider::MeshCollider(json j, GameObject * parent)
{
	this->parent = parent;
	arraySize = 0;
	for (json::iterator vertex = j["vertices"].begin(); vertex != j["vertices"].end(); ++vertex)//process data or overrides
	{
		DirectX::XMVECTOR current = { vertex.value()[0],vertex.value()[1],vertex.value()[2],0 };
		vertices.push_back(current);
		arraySize+=2;
	}
	pointArray = new float[arraySize];
	isTrigger = j["trigger"];
	arrayResized = false;
}

void MeshCollider::Start()
{
	trans = parent->getComponent<Transform>();
	int arrayIndex = 0;
	if (!arrayResized)
	{
		arrayResized = true;
		for (DirectX::XMVECTOR c : vertices)
		{
			//since vertices are within the range of -.5 to .5 they are scaled to make the object bigger. Currently this is based off of transform's scale like in unity.
			//Important note that this never updates with scale like unity does, this should be fixed
			pointArray[arrayIndex + 0] = c.m128_f32[0] * trans->scale.x;
			pointArray[arrayIndex + 1] = c.m128_f32[1] * trans->scale.y;
			arrayIndex += 2;
		}
	}
	}

void MeshCollider::Update()
{
}

int MeshCollider::getCompId()
{
	return ComponentType::MESH_COLLIDER;
}



Component * MeshCollider::Clone(GameObject * newParent)
{
	MeshCollider* toReturn = new MeshCollider();
	toReturn->parent = newParent;
	for (DirectX::XMVECTOR v : vertices)
		toReturn->vertices.push_back(v);
	toReturn->isTrigger = isTrigger;
	toReturn->pointArray = pointArray;
	toReturn->arraySize = arraySize;
	toReturn->arrayResized = arrayResized;
	return (Component*)toReturn;
}

void MeshCollider::CollisionCheck(GameObject* toCheck)
{
	if (toCheck != parent)
	{
		Transform* playerTransform = toCheck->getComponent<Transform>();
		float minDistance = playerTransform->scale.y + trans->scale.y + 500;
		minDistance *= minDistance;
		DirectX::XMVECTOR pT = playerTransform->GetPosXMVector();
		DirectX::XMVECTOR t = trans->GetPosXMVector();
		if (XM2DSquareDistance(&pT, &t) < minDistance)//skip some calculations
		{
			float* actualPointArray = new float[arraySize];
			for (int i = 0; i < arraySize; i += 2)
			{
				actualPointArray[i] = pointArray[i] + trans->GetPosXMVector().m128_f32[0];
				actualPointArray[i + 1] = pointArray[i + 1] + trans->GetPosXMVector().m128_f32[1];
			}

			SquareCollider* toCheckCollider = toCheck->getComponent<SquareCollider>();
			float* playerPointArray = new float[8];
			float* orginPointArray = toCheckCollider->getPoints();
			for (int i = 0; i < 8; i += 2)
			{
				playerPointArray[i] = orginPointArray[i] + playerTransform->GetPosXMVector().m128_f32[0];
				playerPointArray[i + 1] = orginPointArray[i + 1] + playerTransform->GetPosXMVector().m128_f32[1];
			}
			DirectX::XMVECTOR moveDistance;
			int returnVal = SATCollision(actualPointArray, arraySize, playerPointArray, 8, &moveDistance);
			if (returnVal == 1)
			{
				if (isTrigger)
					EventManager::getInstance().BroadcastMessageToSubscribers(new DamageCollisionMessage(parent->tag));
				else
				{
					Vector2D delta;
					Vector2DSet(&delta, 0, 0);
					EventManager::getInstance().BroadcastMessageToSubscribers(new CollisionMessage(parent->tag, delta));
				}
			}
			delete actualPointArray;
			delete playerPointArray;
		}
	}
}
