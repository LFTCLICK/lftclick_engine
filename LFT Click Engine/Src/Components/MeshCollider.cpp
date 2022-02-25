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

MeshCollider::MeshCollider()
{
}

void MeshCollider::Deserialize(nlohmann::json j, GameObject* parent)
{
	this->parent = parent;
	arraySize = 0;
	for (json::iterator vertex = j["vertices"].begin(); vertex != j["vertices"].end(); ++vertex)//process data or overrides
	{
		DirectX::XMVECTOR current = { vertex.value()[0],vertex.value()[1],vertex.value()[2],0 };
		vertices.push_back(current);
		arraySize += 2;
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

int SATCollision(float* p1, int p1Size, float* p2, int p2Size, DirectX::XMVECTOR* moveDistance)
{
	int numberOfCollisions = 0;
	int trialCounter = 0;
	float leastDistanceToMove = FLT_MAX;
	DirectX::XMVECTOR leastDistanceToMoveVec;
	while (trialCounter < 2)
	{
		int i = 0;
		while (i < p1Size)
		{
			DirectX::XMVECTOR currentNormal;
			currentNormal = DirectX::XMVectorSet(-(p1[(i + 1) % p1Size] - p1[(i + 3) % p1Size]), p1[i % p1Size] - p1[(i + 2) % p1Size], 0, 1);//normal of vector is (-(y1-y2), (x1-x2))
			currentNormal = DirectX::XMVectorScale(currentNormal, 1 / DirectX::XMVector3Length(currentNormal).m128_f32[0]);
			int p1MinSATProjection, p2MinSATProjection, p1MaxSATProjection, p2MaxSATProjection;
			p1MinSATProjection = p2MinSATProjection = INT_MAX;
			p1MaxSATProjection = p2MaxSATProjection = INT_MIN;
			int j = 0;
			while (j < p1Size)
			{
				DirectX::XMVECTOR temp;
				temp = DirectX::XMVectorSet(p1[j], p1[j + 1], 0, 1);
				float dotP = DirectX::XMVector3Dot(temp, currentNormal).m128_f32[0];
				if (dotP < p1MinSATProjection)
					p1MinSATProjection = dotP;
				if (dotP > p1MaxSATProjection)
					p1MaxSATProjection = dotP;
				j += 2;
			}
			j = 0;
			while (j < p2Size)
			{
				DirectX::XMVECTOR temp;
				temp = DirectX::XMVectorSet(p2[j], p2[j + 1], 0, 1);
				float dotP = DirectX::XMVector3Dot(temp, currentNormal).m128_f32[0];
				if (dotP < p2MinSATProjection)
					p2MinSATProjection = dotP;
				if (dotP > p2MaxSATProjection)
					p2MaxSATProjection = dotP;
				j += 2;
			}
			if (!(p2MinSATProjection > p1MaxSATProjection || p2MaxSATProjection < p1MinSATProjection))
			{
				if (fabsf(p2MinSATProjection - p1MaxSATProjection) < fabsf(p2MaxSATProjection - p1MinSATProjection))
				{
					if (fabsf(p2MinSATProjection - p1MaxSATProjection) < fabsf(leastDistanceToMove))
					{
						leastDistanceToMove = p2MinSATProjection - p1MaxSATProjection;
						leastDistanceToMoveVec = DirectX::XMVectorSet(currentNormal.m128_f32[0], currentNormal.m128_f32[1], 0, 1);
						if (trialCounter != 0)
							leastDistanceToMove *= -1;
					}
				}
				else
				{
					if (fabsf(p2MaxSATProjection - p1MinSATProjection) < fabsf(leastDistanceToMove))
					{
						leastDistanceToMove = p2MaxSATProjection - p1MinSATProjection;
						leastDistanceToMoveVec = DirectX::XMVectorSet(currentNormal.m128_f32[0], currentNormal.m128_f32[1], 0, 1);
						if (trialCounter == 0)
							leastDistanceToMove *= -1;
					}
				}
				numberOfCollisions++;
			}
			i += 2;
		}
		trialCounter++;
		int tempSize = p1Size;
		p1Size = p2Size;
		p2Size = tempSize;
		float* tempP;
		tempP = p1;
		p1 = p2;
		p2 = tempP;
	}
	if ((p1Size + p2Size) / 2 == numberOfCollisions)
	{
		*moveDistance = DirectX::XMVectorScale(leastDistanceToMoveVec, leastDistanceToMove);
		return 1;
	}
	return 0;
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

		if (DirectX::SimpleMath::Vector2::DistanceSquared(pT, t) < minDistance)//skip some calculations
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
					DirectX::SimpleMath::Vector2 delta(0.0f, 0.0f);
					EventManager::getInstance().BroadcastMessageToSubscribers(new CollisionMessage(parent->tag, delta));
				}
			}
			delete actualPointArray;
			delete playerPointArray;
		}
	}
}
