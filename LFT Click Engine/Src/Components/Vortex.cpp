#include "pch.h"
#include "Vortex.h"
#include "FrameRateController.h"
#include "Collider.h"

void Vortex::Start()
{
	playerTrans = g_GameObjManager->FindObjectOfTag("player")->getComponent<Transform>();
	ownerTrans = componentOwner->getComponent<Transform>();
}

void Vortex::Update()
{
	float distance = DirectX::SimpleMath::Vector2::Distance(playerTrans->CurrentPos(), ownerTrans->CurrentPos());

	if (distance < pullingRadius)
	{
		DirectX::SimpleMath::Vector2 moveVector = ownerTrans->CurrentPos() - playerTrans->CurrentPos();
		moveVector.Normalize();
		playerTrans->Move(250 * moveVector.x * g_FrameRateController->DeltaTime(), 250 * moveVector.y * g_FrameRateController->DeltaTime());
	}
}

Component* Vortex::Clone(GameObject* newParent)
{
	Vortex* toReturn = new Vortex();
	toReturn->componentOwner = newParent;
	toReturn->pullingRadius = pullingRadius;
	return (Component*)toReturn;
}

void Vortex::Deserialize(nlohmann::json j, GameObject* componentOwner)
{
	this->componentOwner = componentOwner;
	pullingRadius = j["PullingRadius"];
}


void Vortex::HandleMessage(Message* e)
{
	if (e->otherObject->componentOwner->tag == "player")
	{
		playerInRange = true;
	}
}