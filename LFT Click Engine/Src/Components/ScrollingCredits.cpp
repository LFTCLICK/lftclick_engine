#include "pch.h"
#include "ScrollingCredits.h"

void ScrollingCredits::Start()
{
	myTransform = componentOwner->getComponent<Transform>();
	componentOwner->isActive = false;
}

void ScrollingCredits::Update()
{
	myTransform->Move(0, scrollSpeed * g_FrameRateController->TrueDeltaTime());
	if (myTransform->position.y >= endingY)
		myTransform->SetPos(myTransform->position.x, endingY);
}

Component* ScrollingCredits::Clone(GameObject* newParent)
{
	ScrollingCredits* toReturn = new ScrollingCredits();
	toReturn->startingPos = startingPos;
	toReturn->scrollSpeed = scrollSpeed;
	toReturn->endingY = endingY;
	toReturn->componentOwner = newParent;
	return (Component*)toReturn;
}

void ScrollingCredits::Deserialize(nlohmann::json j, GameObject* parent)
{
	this->componentOwner = parent;
	std::vector<float> centerHelper = j["startingPos"].get<std::vector<float>>();
	startingPos = { centerHelper[0], centerHelper[1] };
	scrollSpeed = j["scrollSpeed"];
	endingY = j["endingY"];
}

void ScrollingCredits::reset()
{
	myTransform->SetPos(startingPos);
}
