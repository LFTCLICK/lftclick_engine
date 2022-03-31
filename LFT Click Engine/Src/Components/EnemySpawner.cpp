#include "pch.h"
#include "EnemySpawner.h"
#include "./FrameRateController.h"
#include "Enemy.h"
#include "GameManager.h"

void EnemySpawner::Start()
{
	positiveBounds = Vector2(1280, 7680);
	negativeBounds = Vector2(-1280, -7680);
	spawnTimer = timeBetweenPhases;
	myPos = componentOwner->getComponent<Transform>()->CurrentPos();
}

void EnemySpawner::Update()
{
	spawnTimer -= g_FrameRateController->DeltaTime();

	if (spawnTimer <= 0.0f)
	{
		spawnTimer = timeBetweenPhases;

		if (objectSpawnName == "zombie")
		{
			int toSpawn;
			// checking if its morning time
			if (g_GameManager->time < SUN_SETTING || g_GameManager->time > SUN_RISING)
				toSpawn = 10.0f;
			else
				toSpawn = 30.0f;

			totalZombiesOnMap += toSpawn;
			for (int i = 0; i < toSpawn; i++)
			{
				GameObject* newObj = g_GameObjManager->ClonePrefabOfTag(g_GameObjFactory.get(), objectSpawnName);
				newObj->getComponent<Transform>()->SetPos(randomNumberGetter(), randomNumberGetter());
			}
			printf("Total zombies on map are %d\n", totalZombiesOnMap);
		}
		
		if (objectSpawnName == "enemy")
		{
			ghostsOnMap += 1;
			GameObject* newObj = g_GameObjManager->ClonePrefabOfTag(g_GameObjFactory.get(), objectSpawnName);
			newObj->getComponent<Transform>()->SetPos(randomNumberGetter(), randomNumberGetter());
			printf("Total ghosts on map are %d\n", ghostsOnMap);
		}

		++x;
	}
}

void EnemySpawner::Deserialize(nlohmann::json j, GameObject* componentOwner)
{
	this->componentOwner = componentOwner;
	slope = j["slope"];
	c = j["c"];
	timeBetweenPhases = j["timeBetweenPhases"];
	if (j.contains("objectSpawnName")) objectSpawnName = j["objectSpawnName"];
}

float EnemySpawner::randomNumberGetter()
{
	int value = rand() % 2;
	if (value == 0) return positiveBounds.x + static_cast <float> (std::rand() / (static_cast <float> (RAND_MAX / (positiveBounds.y - positiveBounds.x))));
	else if (value == 1)
		return negativeBounds.y + static_cast <float> (std::rand() / (static_cast <float> (RAND_MAX / (negativeBounds.x - negativeBounds.y))));
}

Component* EnemySpawner::Clone(GameObject* newParent)
{
	EnemySpawner* toReturn = new EnemySpawner();
	toReturn->slope = slope;
	toReturn->c = c;
	toReturn->timeBetweenPhases = timeBetweenPhases;
	toReturn->objectSpawnName = objectSpawnName;
	toReturn->componentOwner = newParent;
	return toReturn;
}

//newObj->getComponent<Transform>()->SetPos(myPos.x + (rand() % (int)bounds.x) - (bounds.x / 2), myPos.y +
//	(rand() % (int)bounds.y) - (bounds.y / 2));

//slope* x + c