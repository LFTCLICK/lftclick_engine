// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	Audible.cpp
// Author			:	Christian Fitch
// Creation Date	:	2022/02/10
// Purpose			:	Component to play sounds using audio manager
// History			:
// ---------------------------------------------------------------------------

#include "Audible.h"

using json = nlohmann::json;

void Audible::Start() {
	if (backgroundMusic.length() > 0) {
		AudioManager::getInstance().Play(backgroundMusic);
		std::cout << "AUDIO PLAY" << std::endl;
	}
	std::cout << "AUDIO START" << std::endl;
}

void Audible::Update() {
	std::cout << "AUDIO UPDATE" << std::endl;
}

Component* Audible::Clone(GameObject* newParent) {
	Audible* toReturn = new Audible();
	return toReturn;
}

Audible::Audible(json j, GameObject* parent) : parent(parent), backgroundMusic("") {
	if (j.contains("backgroundMusic")) {
		backgroundMusic = j["backgroundMusic"];
		AudioManager::getInstance().Play(backgroundMusic);
	}
}