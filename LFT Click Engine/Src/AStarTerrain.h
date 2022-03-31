#pragma once
#include "pch.h"

struct Node
{
	float timeLastUpdated;
	float gCost;
	float fCost;
	Node* parent;
	Node* nextTileInPath;

};
struct GridSpace
{
	int row;
	int col;
};

class AStarTerrain
{
public:
	int** terrain;
	Node** nodeMap;
	float tileSize;
	int width, height;

	GridSpace WorldToGridPos(DirectX::SimpleMath::Vector2 pos);

	AStarTerrain() = default;
	~AStarTerrain();

};