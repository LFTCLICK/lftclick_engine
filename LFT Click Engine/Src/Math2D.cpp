// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	Math2D.cpp
// Author			:	Vance Howald
// Creation Date	:	2021/09/08
// Purpose			:	c implementation for collision testing
// History			:
// 2021/09/13		-	initial implementation
// 2021/10/07		-	Added SAT CollisionMessage
// 2021/11/21		-	Switched to XMVECTOR
// 2021/12/01		-	Fixed SAT
// ---------------------------------------------------------------------------

#include "pch.h"
#include "Math2D.h"
#include "stdio.h"
#include <limits.h>
#include <float.h>

float XM2DSquareDistance(DirectX::XMVECTOR *pVec0, DirectX::XMVECTOR *pVec1)
{
	return ((pVec1->m128_f32[0] - pVec0->m128_f32[0]) * (pVec1->m128_f32[0] - pVec0->m128_f32[0])) + ((pVec1->m128_f32[1] - pVec0->m128_f32[1]) * (pVec1->m128_f32[1] - pVec0->m128_f32[1]));
}

/*
This function checks if the point P is colliding with the circle whose
center is "Center" and radius is "Radius"
*/
int StaticPointToStaticCircle(DirectX::XMVECTOR *pP, DirectX::XMVECTOR *pCenter, float Radius)
{
	return XM2DSquareDistance(pP, pCenter) <= Radius* Radius;
}


/*
This function checks if the point Pos is colliding with the rectangle
whose center is Rect, width is "Width" and height is Height
*/
int StaticPointToStaticRect(DirectX::XMVECTOR *pPos, DirectX::XMVECTOR *pRect, float Width, float Height)
{
	if (pPos->m128_f32[0] < pRect->m128_f32[0] - Width / 2)//to the left
		return 0;
	if (pPos->m128_f32[1] < pRect->m128_f32[1] - Height / 2)//below
		return 0;
	if (pPos->m128_f32[0] > pRect->m128_f32[0] + Width/2)//to the right
		return 0;
	if (pPos->m128_f32[1] > pRect->m128_f32[1] + Height/2)//above
		return 0;
  return 1;
}

/*
This function checks for collision between 2 circles.
Circle0: Center is Center0, radius is "Radius0"
Circle1: Center is Center1, radius is "Radius1"
*/
int StaticCircleToStaticCircle(DirectX::XMVECTOR *pCenter0, float Radius0, DirectX::XMVECTOR *pCenter1, float Radius1)
{
  return XM2DSquareDistance(pCenter0, pCenter1) <= (Radius0 + Radius1) * (Radius0 + Radius1);
}

/*
This functions checks if 2 rectangles are colliding
Rectangle0: Center is pRect0, width is "Width0" and height is "Height0"
Rectangle1: Center is pRect1, width is "Width1" and height is "Height1"
*/
int StaticRectToStaticRect(DirectX::XMVECTOR *pRect0, float Width0, float Height0, DirectX::XMVECTOR *pRect1, float Width1, float Height1)
{
	if (pRect0->m128_f32[0] - Width0/2 > pRect1->m128_f32[0] + Width1 / 2)//leftA > rightB
		return 0;
	if (pRect0->m128_f32[0] + Width0 / 2 < pRect1->m128_f32[0] - Width1 / 2)//rightA < leftB
		return 0;
	if (pRect0->m128_f32[1] + Height0 / 2 < pRect1->m128_f32[1] - Height1 / 2)//topA < bottomB
		return 0;
	if (pRect0->m128_f32[1] - Height0 / 2 > pRect1->m128_f32[1] + Height1 / 2)//bottomA > topB
		return 0;
	return 1;
}

int SATCollision(float* p1, int p1Size, float* p2, int p2Size, DirectX::XMVECTOR* moveDistance)
{
	int numberOfCollisions = 0;
	int trialCounter=0;
	float leastDistanceToMove= FLT_MAX;
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
