// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	AStarTerrain.cpp
// Author			:	Vance Howald
// Creation Date	:	2021/03/23
// Purpose			:	Terrain data for A* 
// History			:
// ---------------------------------------------------------------------------

#include "pch.h"
#include "AStarTerrain.h"


GridSpace AStarTerrain::WorldToGridPos(DirectX::SimpleMath::Vector2 pos)
{
    // int mapX = (x - (width / 2)) * objectSize, mapY = (y - (height / 2)) * objectSize * -1;
    int xPos = (pos.x / tileSize) + (width / 2) - .5f;
    int yPos = (-pos.y / tileSize) + (height / 2) - .5f;
    return GridSpace{ yPos,xPos };
}

AStarTerrain::~AStarTerrain()
{
    for (int i = 0; i < width; i++)
    {
        delete terrain[i];
        delete nodeMap[i];
    }
    delete[] terrain;
    delete[] nodeMap;
}
