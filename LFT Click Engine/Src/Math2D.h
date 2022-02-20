// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	Math2D.h
// Author			:	Vance Howald
// Creation Date	:	2021/09/08
// Purpose			:	header for collision testing
// History			:
// - 2021/09/08		:	- initial implementation
// - 2021/10/07		:	Added SAT Collision
// ---------------------------------------------------------------------------

#ifndef MATH2D_H
#define MATH2D_H
#include <DirectXMath.h>

float XM2DSquareDistance(DirectX::XMVECTOR *pVec0, DirectX::XMVECTOR *pVec1);

/*
This function checks if the point P is colliding with the circle whose
center is "Center" and radius is "Radius"
*/
int StaticPointToStaticCircle(DirectX::XMVECTOR *pP, DirectX::XMVECTOR *pCenter, float Radius);


/*
This function checks if the point Pos is colliding with the rectangle
whose center is Rect, width is "Width" and height is Height
*/
int StaticPointToStaticRect(DirectX::XMVECTOR *pPos, DirectX::XMVECTOR *pRect, float Width, float Height);

/*
This function checks for collision between 2 circles.
Circle0: Center is Center0, radius is "Radius0"
Circle1: Center is Center1, radius is "Radius1"
*/
int StaticCircleToStaticCircle(DirectX::XMVECTOR *pCenter0, float Radius0, DirectX::XMVECTOR *pCenter1, float Radius1);

/*
This functions checks if 2 rectangles are colliding
Rectangle0: Center is pRect0, width is "Width0" and height is "Height0"
Rectangle1: Center is pRect1, width is "Width1" and height is "Height1"
*/
int StaticRectToStaticRect(DirectX::XMVECTOR *pRect0, float Width0, float Height0, DirectX::XMVECTOR *pRect1, float Width1, float Height1);

int SATCollision(float* p1, int p1Size, float* p2, int p2Size, DirectX::XMVECTOR* moveDistance);

#endif