// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	AStarTerrain.h
// Author			:	Vance Howald
// Creation Date	:	2021/03/23
// Purpose			:	Terrain data for A* 
// History			:
// ---------------------------------------------------------------------------

#pragma once
#include "pch.h"

struct GridPos
{
	int row;
	int col;

	bool operator==(const GridPos& other) const
	{
		return row == other.row && col == other.col;
	}
};
struct Node
{
	GridPos pos;
	bool onCloseList;
	float gCost;
	float fCost;
	Node* parent;
	DirectX::SimpleMath::Vector3 vec3;
	int currentItr;
};



class AStarTerrain
{
private:
	float Octile(GridPos* start, GridPos* end);
	struct posComparisonHelper {
		bool operator()(const Node* a, const Node* b)
		{
			return a->fCost > b->fCost;
		}
	};
public:
	int** terrain;
	Node*** nodeMap;
	float tileSize;
	int clientWidth, clientHeight;
	int trueItr;
	AStarTerrain() = default;
	~AStarTerrain();

	void Init();
	GridPos WorldToGridPos(DirectX::SimpleMath::Vector2 pos);
	int ComputePath(GridPos* start, GridPos* goal, std::list<DirectX::SimpleMath::Vector2>& path);
};

extern std::unique_ptr<AStarTerrain> g_AStarTerrain;