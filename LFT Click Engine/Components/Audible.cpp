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
	channelGroupName = parent->tag;
	am->LoadChannelGroup(channelGroupName);

	if (sounds.empty())
		std::cout << "Audible component is empty." << std::endl;
	else
		for (auto sound : sounds) {
			am->LoadSound(sound.name, sound.loop, sound.compressed);
			if (std::find(sound.playEvents.begin(), sound.playEvents.end(), PLAY_ON_START) != sound.playEvents.end())
				PlaySound(sound);
		}
}

void Audible::Update() {
	for (auto channel = channels.begin(); channel != channels.end();)
		if (!am->IsPaused(*channel) && !am->IsPlaying(*channel))
			channel = channels.erase(channel);
		else
			++channel;
	/*
	Vector2D finalPos = { position.x, position.y };
	if (positionOffset.x == 0 && positionOffset.y == 0)
		Vector2DAdd(&finalPos, &finalPos, &positionOffset);
	SetPosition(finalPos);
	*/
}

Component* Audible::Clone(GameObject* newParent) {
	Audible* toReturn = new Audible();
	return toReturn;
}

Audible::Audible(json j, GameObject* parent) : parent(parent), am(&AudioManager::getInstance()), sounds({}), position({ 0, 0 }), positionOffset({ 0, 0 }) {
	if (j.contains("sounds")) {
		for (auto it = std::begin(j["sounds"]); it != std::end(j["sounds"]); it++) {
			SoundInfo soundInfo;
			auto sound = *it;

			if (sound.contains("name")) soundInfo.name = sound["name"];
			if (sound.contains("loop")) soundInfo.loop = sound["loop"];
			if (sound.contains("compressed")) soundInfo.compressed = sound["compressed"];
			if (sound.contains("volume")) soundInfo.volume = sound["volume"];
			if (sound.contains("playEvents"))
				soundInfo.playEvents.insert(soundInfo.playEvents.begin(), std::begin(sound["playEvents"]), std::end(sound["playEvents"]));
			if (sound.contains("stopEvents"))
				soundInfo.stopEvents.insert(soundInfo.stopEvents.begin(), std::begin(sound["stopEvents"]), std::end(sound["stopEvents"]));

			sounds.push_back(soundInfo);
		}
	}
}




void Audible::PlaySound(SoundInfo sound) {
	am->PlaySound(sound.name, channelGroupName, sound.volume);
}

void Audible::Unpause() {
	am->UnpauseGroup(channelGroupName);
}

void Audible::Pause() {
	am->PauseGroup(channelGroupName);
}

void Audible::SetPaused(bool paused) {
	am->SetGroupPaused(channelGroupName, paused);
}

void Audible::SetVolume(float volume) {
	am->SetGroupVolume(channelGroupName, volume);
}

void Audible::Mute() {
	am->MuteGroup(channelGroupName);
}


float Audible::GetVolume() {
	return am->GetGroupVolume(channelGroupName);
}

Vector2D Audible::GetPosition() {
	return am->GetGroupPosition(channelGroupName);
}

void Audible::SetPosition(float x, float y) {
	am->SetGroupPosition(channelGroupName, x, y);
}
void Audible::SetPosition(Vector2D position) {
	am->SetGroupPosition(channelGroupName, position);
}

float Audible::GetPitch() {
	return am->GetGroupPitch(channelGroupName);
}

void Audible::SetPitch(float pitch) {
	am->SetGroupPitch(channelGroupName, pitch);
}

bool Audible::IsPlaying() {
	return am->IsGroupPlaying(channelGroupName);
}

void Audible::Stop() {
	am->StopGroup(channelGroupName);
}



void Audible::HandleMessage(Message* e) {

}