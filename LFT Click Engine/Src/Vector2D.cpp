// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	Vector2D.cpp
// Author			:	Vance Howald
// Creation Date	:	2021/09/24
// Purpose			:	c implementation for 2D matrix opperations
// History			:
// - 2021/09/13		:	- initial implementation
// - 2021/09/29		:	- added vector to angle
// ---------------------------------------------------------------------------

#include "Vector2D.h"

#define EPSILON 0.0001

////////////////////
// From Project 2 //
////////////////////

// ---------------------------------------------------------------------------

void Vector2DZero(Vector2D* pResult)
{
	pResult->x = 0;
	pResult->y = 0;
}

// ---------------------------------------------------------------------------

void Vector2DSet(Vector2D* pResult, float x, float y)
{
	pResult->x = x;
	pResult->y = y;
}

// ---------------------------------------------------------------------------

void Vector2DNeg(Vector2D* pResult, Vector2D* pVec0)
{
	Vector2D temp;
	temp.x = -pVec0->x;
	temp.y = -pVec0->y;
	Vector2DSet(pResult, temp.x, temp.y);
}

// ---------------------------------------------------------------------------

void Vector2DAdd(Vector2D* pResult, Vector2D* pVec0, Vector2D* pVec1)
{
	Vector2D temp;
	temp.x = pVec0->x + pVec1->x;
	temp.y = pVec0->y + pVec1->y;
	Vector2DSet(pResult, temp.x, temp.y);
}

// ---------------------------------------------------------------------------

void Vector2DSub(Vector2D* pResult, Vector2D* pVec0, Vector2D* pVec1)
{
	Vector2D temp;
	temp.x = pVec0->x - pVec1->x;
	temp.y = pVec0->y - pVec1->y;
	Vector2DSet(pResult, temp.x, temp.y);
}

// ---------------------------------------------------------------------------

void Vector2DNormalize(Vector2D* pResult, Vector2D* pVec0)
{
	Vector2D temp;
	float distance = sqrtf((pVec0->x * pVec0->x) + (pVec0->y * pVec0->y));
	temp.x = pVec0->x / distance;
	temp.y = pVec0->y / distance;
	Vector2DSet(pResult, temp.x, temp.y);
}

// ---------------------------------------------------------------------------

void Vector2DScale(Vector2D* pResult, Vector2D* pVec0, float c)
{
	Vector2D temp;
	temp.x = c * pVec0->x;
	temp.y = c * pVec0->y;
	Vector2DSet(pResult, temp.x, temp.y);
}

// ---------------------------------------------------------------------------

void Vector2DScaleAdd(Vector2D* pResult, Vector2D* pVec0, Vector2D* pVec1, float c)
{
	Vector2D temp;
	Vector2DScale(&temp, pVec0, c);
	Vector2DAdd(pResult, &temp, pVec1);
}

// ---------------------------------------------------------------------------

void Vector2DScaleSub(Vector2D* pResult, Vector2D* pVec0, Vector2D* pVec1, float c)
{
	Vector2DScale(pResult, pVec0, c);
	Vector2DSub(pResult, pResult, pVec1);
}

// ---------------------------------------------------------------------------

float Vector2DLength(Vector2D* pVec0)
{
	return sqrtf(Vector2DSquareLength(pVec0));
}

// ---------------------------------------------------------------------------

float Vector2DSquareLength(Vector2D* pVec0)
{
	return (pVec0->x * pVec0->x) + (pVec0->y * pVec0->y);
}

// ---------------------------------------------------------------------------

float Vector2DDistance(Vector2D* pVec0, Vector2D* pVec1)
{
	return sqrtf(Vector2DSquareDistance(pVec0, pVec1));
}

// ---------------------------------------------------------------------------

float Vector2DSquareDistance(Vector2D* pVec0, Vector2D* pVec1)
{
	return ((pVec1->x - pVec0->x) * (pVec1->x - pVec0->x)) + ((pVec1->y - pVec0->y) * (pVec1->y - pVec0->y));
}

// ---------------------------------------------------------------------------

float Vector2DDotProduct(Vector2D* pVec0, Vector2D* pVec1)
{
	return (pVec0->x * pVec1->x) + (pVec0->y * pVec1->y);
}

float Vector2DRadFromVector2D(Vector2D* pVec0)
{
	return atan2f(pVec0->y, pVec0->x);
}

float Vector2DDegreeFromVector2D(Vector2D* pVec0)
{
	return Vector2DRadFromVector2D(pVec0) * 90.0f / 1.57079632679f;
}

float Vector2DMagnitude(Vector2D* pVec0)
{
	return sqrtf((pVec0->x * pVec0->x) + (pVec0->y * pVec0->y));
}

// ---------------------------------------------------------------------------

void Vector2DFromAngleDeg(Vector2D* pResult, float angle)
{
	float HALF_PI = 1.57079632679f;
	pResult->x = cosf(angle * (HALF_PI / 90.0f));
	pResult->y = cosf(angle * (HALF_PI / 90.0f));
}

// ---------------------------------------------------------------------------

void Vector2DFromAngleRad(Vector2D* pResult, float angle)
{
	pResult->x = sinf(angle);
	pResult->y = cosf(angle);
}

// ---------------------------------------------------------------------------
