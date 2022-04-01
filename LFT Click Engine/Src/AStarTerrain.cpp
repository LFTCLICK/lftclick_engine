#include "pch.h"
#include <queue>
#include "AStarTerrain.h"

#define SQRT_2 1.41421356237


void AStarTerrain::Init()
{

    for (int row = 0; row < height; ++row)
    {
        for (int col = 0; col < width; ++col)
        {
            Node* current = nodeMap[row][col];
            current->gCost = FLT_MAX;
            current->fCost = FLT_MAX;
            current->currentItr = -1;
            if (terrain[row][col] == -1)
            {
                current->gCost = -10;
                current->currentItr = INT_MAX;
            }
            current->parent = nullptr;
            current->onCloseList = false;

        }
    }
    trueItr = 0;
}

GridPos AStarTerrain::WorldToGridPos(DirectX::SimpleMath::Vector2 pos)
{
    //((x - (width / 2)) * objectSize, (y - (height / 2)) * objectSize * -1, 0.0f);
    // int mapX = (x - (width / 2)) * objectSize, mapY = (y - (height / 2)) * objectSize * -1;
    int xPos = (pos.x / tileSize) + (width / 2) - .5f;
    if(xPos<0)
        xPos = (pos.x / tileSize) + (width / 2) - .5f;
    int yPos = (-pos.y / tileSize) + (height / 2) - .5f;
    return GridPos{ yPos+1,xPos+1 };
}

int AStarTerrain::ComputePath(GridPos* start, GridPos* goal, std::list<DirectX::SimpleMath::Vector2>& path)
{
    std::priority_queue<Node*, std::vector<Node*>, posComparisonHelper>  openList = std::priority_queue<Node*, std::vector<Node*>, posComparisonHelper>();
    if (start->row < 0 || start->row>= height || start->col>= width)
        return -1;

    //for (int row = 0; row < height; ++row)
    //{
    //    for (int col = 0; col < width; ++col)
    //    {
    //        Node* current = nodeMap[row][col];
    //        current->gCost = FLT_MAX;
    //        current->fCost = FLT_MAX;
    //        if (terrain[row][col] == -1)
    //            current->gCost = -10;
    //        current->parent = nullptr;
    //        current->onCloseList = false;
    //
    //    }
    //}
    trueItr++;
    Node* startingPos = nodeMap[start->row][start->col];
    startingPos->gCost = 0;
    startingPos->fCost = Octile(start, goal);
    startingPos->currentItr = trueItr;
    startingPos->parent = nullptr;
    startingPos->onCloseList = false;
    openList.push(startingPos);
    while (!openList.empty())
    {
        Node* current = openList.top();
        openList.pop();
        if (current->onCloseList)
            continue;
        if (current->pos == *goal)
        {
            //do rubberbanding here
            if (current->parent != nullptr)
            {
                Node* headIndex = current;
                Node* tailIndex = current->parent->parent;
                while (tailIndex != nullptr)
                {
                    int numberOfRowsToProcess = abs(headIndex->pos.row - tailIndex->pos.row);
                    int numberOfColsToProcess = abs(headIndex->pos.col - tailIndex->pos.col);
                    int startingRow = headIndex->pos.row < tailIndex->pos.row ? headIndex->pos.row : tailIndex->pos.row;
                    int startingCol = headIndex->pos.col < tailIndex->pos.col ? headIndex->pos.col : tailIndex->pos.col;
                    bool shouldRemove = true;
                    for (int deltaRow = 0; deltaRow <= numberOfRowsToProcess; deltaRow++)
                    {
                        for (int deltaCol = 0; deltaCol <= numberOfColsToProcess; deltaCol++)
                        {
                            if (nodeMap[startingRow + deltaRow][startingCol + deltaCol]->gCost < 0)
                            {
                                shouldRemove = false;
                                deltaCol = numberOfColsToProcess + 1;
                                deltaRow = numberOfRowsToProcess + 1;
                            }
                        }
                    }
                    if (shouldRemove)
                    {
                        headIndex->parent = headIndex->parent->parent;
                        tailIndex = tailIndex->parent;
                    }
                    else
                    {
                        headIndex = headIndex->parent;
                        tailIndex = tailIndex->parent;
                    }
                }
            }
            int toReturn = -1;
            do
            {
                path.push_front(DirectX::SimpleMath::Vector2(current->vec3.x, current->vec3.y));
                current = current->parent;
                ++toReturn;
            } while (current != nullptr);
            path.pop_front();
            return toReturn;
            //if (request.settings.rubberBanding && request.settings.smoothing)
            //{
            //    auto it = request.path.begin();
            //    while (true)
            //    {
            //        if (it == request.path.end())
            //            break;
            //        Vec3 pointA = *it;
            //        it++;
            //        if (it == request.path.end())
            //            break;
            //        Vec3 pointB = *it;
            //        if (Vec3::Distance(pointA, pointB) > 1.5 * gridSize)
            //        {
            //            Vec3 midPoint = (pointA + pointB) / 2;
            //            request.path.insert(it, midPoint);
            //            it--;
            //            it--;
            //        }
            //    }
            //}
            //if (request.settings.smoothing)
            //{
            //    bool breakOnNext = false;
            //    auto it = request.path.begin();
            //    Vec3 p1 = *it;
            //    Vec3 p2 = *it;
            //    Vec3 p3 = *it;
            //    Vec3 p4 = *it;
            //    while (!breakOnNext)
            //    {
            //
            //        p1 = p2;
            //        p2 = p3;
            //        it++;
            //        p3 = *it;
            //        it++;
            //        if (it == request.path.end())
            //        {
            //            breakOnNext = true;
            //            p4 = p3;
            //        }
            //        else
            //        {
            //            p4 = *it;
            //        }
            //        it--;
            //        request.path.insert(it, Vec3::CatmullRom(p1, p2, p3, p4, .25f));
            //        request.path.insert(it, Vec3::CatmullRom(p1, p2, p3, p4, .5f));
            //        request.path.insert(it, Vec3::CatmullRom(p1, p2, p3, p4, .75f));
            //    }
            //}
            //firstRun = true;
            //return PathResult::COMPLETE;
        }
        for (int deltaRow = -1; deltaRow <= 1; deltaRow++)
        {
            for (int deltaCol = -1; deltaCol <= 1; deltaCol++)
            {
                float newGCost = current->gCost + 1;
                if (deltaRow == deltaCol && deltaRow == 0)
                    continue;

                //if (deltaRow * deltaCol != 0)
                //    continue;
                if (deltaRow + current->pos.row >= width || deltaRow + current->pos.row < 0 || deltaCol + current->pos.col >= height || deltaCol + current->pos.col < 0)
                    continue;
                Node* newSpace = nodeMap[deltaRow + current->pos.row][current->pos.col + deltaCol];

                if (newSpace->currentItr < trueItr)
                {
                    newSpace->gCost = FLT_MAX;
                    newSpace->fCost = FLT_MAX;
                    newSpace->currentItr = trueItr;
                    newSpace->parent = nullptr;
                    newSpace->onCloseList = false;
                }

                if ((deltaRow * deltaCol != 0 && newSpace->gCost > current->gCost + SQRT_2 + .01f) || (deltaRow * deltaCol == 0 && newSpace->gCost > newGCost + .01f))
                {
                    if (deltaRow * deltaCol != 0)
                    {
                        bool useDiag = true;
                        for (int diagHelperRow = -1; diagHelperRow <= 1; diagHelperRow++)
                        {
                            for (int diagHelperCol = -1; diagHelperCol <= 1; diagHelperCol++)
                            {
                                if (diagHelperRow * diagHelperCol == 0)
                                {
                                    if (diagHelperRow + deltaRow < -1 || diagHelperRow + deltaRow > 1 || diagHelperCol + deltaCol < -1 || diagHelperCol + deltaCol > 1 || (diagHelperRow == diagHelperCol && diagHelperRow == 0))
                                        continue;
                                    if (diagHelperRow + deltaRow + current->pos.row >= width || diagHelperRow + deltaRow + current->pos.row < 0 || diagHelperCol + deltaCol + current->pos.col >= height || diagHelperCol + deltaCol + current->pos.col < 0)
                                        continue;
                                    int newRow = current->pos.row + deltaRow + diagHelperRow;
                                    int newCol = ((current->pos.col + diagHelperCol + deltaCol));
                                    if (nodeMap[current->pos.row + deltaRow + diagHelperRow][current->pos.col + diagHelperCol + deltaCol]->gCost < 0)
                                        useDiag = false;
                                }
                            }
                        }
                        if (useDiag)
                            newGCost = current->gCost + SQRT_2;
                        else
                            continue;
                    }

                    if (newSpace->gCost > newGCost + .0001f)
                    {

                        newSpace->fCost = Octile(&newSpace->pos, goal) + newGCost;
                        newSpace->gCost = newGCost;
                        if (newSpace->onCloseList)
                            newSpace->onCloseList = false;
                        newSpace->parent = current;
                        openList.push(newSpace);
                    }
                }
            }
        }
        current->onCloseList = true;
    }
    return -1;
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

float AStarTerrain::Octile(GridPos* start, GridPos* end)
{
    float RowDif = (float)abs(start->col - end->col);
    float ColDif = (float)abs(start->row - end->row);
    float minDif = abs(start->col - end->col) < abs(start->row - end->row) ? (float)abs(start->col - end->col) : (float)abs(start->row - end->row);
    return 1.01f * (minDif * SQRT_2) + (abs(start->col - end->col) > abs(start->row - end->row) ? (float)abs(start->col - end->col) : (float)abs(start->row - end->row)) - minDif;
}
