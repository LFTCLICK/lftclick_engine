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
	int width, height;

	AStarTerrain() = default;
	~AStarTerrain();

	void Init();
	GridPos WorldToGridPos(DirectX::SimpleMath::Vector2 pos);
	int ComputePath(GridPos* start, GridPos* goal, std::list<DirectX::SimpleMath::Vector2>& path);
};