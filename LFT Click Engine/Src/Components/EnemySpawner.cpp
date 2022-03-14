#include "pch.h"
#include "EnemySpawner.h"
#include "./FrameRateController.h"
#include "Enemy.h"

void EnemySpawner::Start()
{
	myPos = parent->getComponent<Transform>()->CurrentPos();
}

void EnemySpawner::Update()
{
	timer += FrameRateController::getInstance().DeltaTime();
	if (timer >= timeBetweenPhases)
	{
		timer = 0;
		int toSpawn = slope * x + c;
		for (int i = 0; i < toSpawn; i++)
		{
			GameObject* newObj = GameObjectManager::getInstance().ClonePrefabOfTag(&GameObjectFactory::getInstance(), objectSpawnName);
			newObj->getComponent<Transform>()->SetPos(myPos.x+(rand()%(int)bounds.x)-(bounds.x/2), myPos.y+ (rand() % (int)bounds.y) - (bounds.y / 2));
			DirectX::SimpleMath::Vector2 temp = myPos + targetPosOffset + DirectX::SimpleMath::Vector2(rand() % 80 - 40, 0);
			newObj->getComponent<Enemy>()->targetBeforePlayer = temp;
		}
		++x;
	}
}

void EnemySpawner::Deserialize(nlohmann::json j, GameObject* parent)
{
	this->parent = parent;
	slope = j["slope"];
	c = j["c"];
	timeBetweenPhases = j["timeBetweenPhases"];
	std::vector<float> spawnBoundsHelper = j["spawnBounds"].get<std::vector<float>>();
	bounds = DirectX::SimpleMath::Vector2(spawnBoundsHelper[0], spawnBoundsHelper[1]);
	std::vector<float> targetHelper = j["targetPosOffset"].get<std::vector<float>>();
	targetPosOffset = DirectX::SimpleMath::Vector2(targetHelper[0], targetHelper[1]);
	if (j.contains("objectSpawnName")) objectSpawnName = j["objectSpawnName"];
}

Component* EnemySpawner::Clone(GameObject* newParent)
{
	EnemySpawner* toReturn = new EnemySpawner();
	toReturn->slope = slope;
	toReturn->c = c;
	toReturn->timeBetweenPhases = timeBetweenPhases;
	toReturn->targetPosOffset = targetPosOffset;
	toReturn->objectSpawnName = objectSpawnName;
	toReturn->bounds = bounds;
	toReturn->parent = newParent;
	return toReturn;
}
