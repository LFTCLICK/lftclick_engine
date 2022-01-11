// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	Matrix2D.cpp
// Author			:	Vance Howald
// Creation Date	:	2021/09/08
// Purpose			:	c implementation for 2D matrix opperations
// History			:
// - 2021/09/13		:	- initial implementation
// ---------------------------------------------------------------------------

#include "Matrix2D.h"


/*
This function sets the matrix Result to the identity matrix
*/
void Matrix2DIdentity(Matrix2D *pResult)
{
	for (int r = 0; r < 3; r++)
		for (int c = 0; c < 3; c++)
			pResult->m[r][c] = r == c ? 1.0f : 0.0f;
}

// ---------------------------------------------------------------------------

/*
This functions calculated the transpose matrix of Mtx and saves it in Result
*/
void Matrix2DTranspose(Matrix2D *pResult, Matrix2D *pMtx)
{
	for (int r = 0; r < 3; r++)
		for (int c = 0; c < 3; c++)
			pResult->m[r][c] = pMtx->m[c][r];
}

// ---------------------------------------------------------------------------

/*
This function multiplies Mtx0 with Mtx1 and saves the result in Result
Result = Mtx0*Mtx1
*/
void Matrix2DConcat(Matrix2D *pResult, Matrix2D *pMtx0, Matrix2D *pMtx1)
{
	Matrix2D helper;//holder for values
	Matrix2DIdentity(&helper);
	for (int r = 0; r < 3; r++)
	{
		for (int c = 0; c < 3; c++)
		{
			float  result = 0;
			for (int i = 0; i < 3; i++)
			{
				float a = pMtx0->m[r][i];
				float b = pMtx1->m[i][c];
				float tot = a * b;
				result += tot;

			}
			helper.m[r][c] = result;
		}
	}
	for (int r = 0; r < 3; r++)
		for (int c = 0; c < 3; c++)
			pResult->m[r][c] = helper.m[r][c];
}

// ---------------------------------------------------------------------------

/*
This function creates a translation matrix from x & y and saves it in Result
*/
void Matrix2DTranslate(Matrix2D *pResult, float x, float y)
{
	Matrix2DIdentity(pResult);
	pResult->m[0][2] = x;
	pResult->m[1][2] = y;
}

// ---------------------------------------------------------------------------

/*
This function creates a scaling matrix from x & y and saves it in Result
*/
void Matrix2DScale(Matrix2D *pResult, float x, float y)
{
	Matrix2DIdentity(pResult);
	pResult->m[0][0] = x;
	pResult->m[1][1] = y;
}

// ---------------------------------------------------------------------------

/*
This matrix creates a rotation matrix from "Angle" whose value is in degree.
Save the resultant matrix in Result
*/
void Matrix2DRotDeg(Matrix2D *pResult, float Angle)
{

	float HALF_PI = 1.57079632679f;
	Angle *= (HALF_PI / 90.0f);//conversion
	Matrix2DIdentity(pResult);
	pResult->m[0][0] = cosf(Angle);
	pResult->m[0][1] = -sinf(Angle);
	pResult->m[1][0] = sinf(Angle);
	pResult->m[1][1] = cosf(Angle);
}

// ---------------------------------------------------------------------------

/*
This matrix creates a rotation matrix from "Angle" whose value is in radian.
Save the resultant matrix in Result
*/
void Matrix2DRotRad(Matrix2D *pResult, float Angle)
{
	Matrix2DIdentity(pResult);
	pResult->m[0][0] = cosf(Angle);
	pResult->m[0][1] = -sinf(Angle);
	pResult->m[1][0] = sinf(Angle);
	pResult->m[1][1] = cos(Angle);
}

// ---------------------------------------------------------------------------

/*
This function multiplies the matrix Mtx with the vector Vec and saves the result in Result
Result = Mtx * Vec
*/
void Matrix2DMultVec(Vector2D *pResult, Matrix2D *pMtx, Vector2D *pVec)
{
	float helper[3] = { pVec->x, pVec->y, 1 };//make looping easier
	float result[3] = { 0,0,0 };//
	for (int i = 0; i < 3; i++)
	{
		float total = 0;
		for (int j = 0; j < 3; j++)
			total += pMtx->m[i][j] * helper[j];
		result[i] = total;
	}
	pResult->x = result[0];
	pResult->y = result[1];
}

// ---------------------------------------------------------------------------
