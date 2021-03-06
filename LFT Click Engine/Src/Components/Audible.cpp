// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	Audible.cpp
// Author			:	Christian Fitch
// Creation Date	:	2022/02/10
// Purpose			:	Component to play sounds using audio manager
// History			:
// ---------------------------------------------------------------------------

#include "pch.h"
#include "Audible.h"

using json = nlohmann::json;

void Audible::Start() {
	channelGroupName = componentOwner->tag + am->GenerateUniqueChannelGroupID();
	am->LoadChannelGroup(channelGroupName);

	if (sounds.empty()) {
		std::cout << "Audible component is empty." << std::endl;
	}
	else {
		LoadSounds(sounds);
		PlaySoundsOnEvent(AUDIO_ON_START);
	}

	myTransform = componentOwner->getComponent<Transform>();

	if (generateRandomTime)
		randomTime = ((float)rand() / RAND_MAX) * DAY_LENGTH;
}

void Audible::LoadSounds(std::vector<SoundInfo> newSounds) {
	for (auto sound : newSounds) {
		am->LoadSound(sound.name, sound.loop, sound.compressed);
	}
}

void Audible::Update() {
	for (auto channel = channels.begin(); channel != channels.end();) {
		if (!am->IsPaused(channel->first) && !am->IsPlaying(channel->first)) {
			channel = channels.erase(channel);
		} else {
			++channel;
		}
		if (dangerScaleChannel > -1) {
			am->SetVolume(dangerScaleChannel, g_GameManager->GetDangerLevel() * dangerScaleChannelVolumeMax);
		}

		//if (channel->second.find("Background") != std::string::npos) std::cout << "========== backgroundpos: " << am->GetSpatialPosition(channel->first).x << std::endl;
	}

	if (myTransform != nullptr && g_GameManager->playerTrans != nullptr) {
		auto pos = myTransform->CurrentPos();
		auto playerPos = g_GameManager->playerTrans->CurrentPos();

		if (myTransform->isMoving != myTransform->wasMoving || g_GameManager->playerTrans->isMoving != g_GameManager->playerTrans->wasMoving) {
			HandleSoundsOnEvent(myTransform->isMoving ? AUDIO_ON_MOVE : AUDIO_ON_HALT);
			if (!myTransform->isMoving && pos.x < 10000000.f && pos.x > -10000000.f) {
				am->SetGroupSpatialPosition(channelGroupName, pos, { 0, 0 });
			}
		}

		if ((myTransform->isMoving || myTransform->wasMoving || g_GameManager->playerTrans->isMoving || g_GameManager->playerTrans->wasMoving) && pos.x < 10000000.f && pos.x > -10000000.f) {
			am->SetGroupSpatialPosition(channelGroupName, pos /*, trans->lastMovement / (1000 / frc->DeltaTime())*/);
		}
	}

	if (generateRandomTime && g_GameManager->time < randomTime && g_GameManager->time + g_FrameRateController->DeltaTime() >= randomTime)
		HandleSoundsOnEvent(AUDIO_ON_RANDOM);
}

Component* Audible::Clone(GameObject* newParent) {
	Audible* toReturn = new Audible();
	toReturn->componentOwner = newParent;
	toReturn->am = g_AudioManager.get();
	toReturn->sounds = sounds;
	toReturn->positionless = positionless;
	toReturn->generateRandomTime = generateRandomTime;
	return toReturn;
}

Audible::~Audible() {
	StopSoundsOnEvent(AUDIO_ON_DELETE);
}




void Audible::PlaySound(SoundInfo sound) {
	float pitch = sound.pitchRange[0] == sound.pitchRange[1] ? 1 : Helpers::randWithinRange(sound.pitchRange[0], sound.pitchRange[1]);
	int channelID = am->PlaySound(sound.name, sound.isMusic ? MUSIC_MASTER_CHANNEL_GROUP : channelGroupName, sound.volume, pitch, sound.startTime);
	channels[channelID] = sound.name;

	float vol;

	if (sound.scaleVolumeWithDanger) {
		dangerScaleChannel = channelID;
		dangerScaleChannelVolumeMax = sound.volume;
	}
	if (sound.isMusic)
		am->SetVolume(channelID, sound.volume);
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

DirectX::SimpleMath::Vector2 Audible::GetPosition() {
	return am->GetGroupSpatialPosition(channelGroupName);
}

void Audible::SetPosition(float x, float y) {
	am->SetGroupSpatialPosition(channelGroupName, x, y);
}
void Audible::SetPosition(DirectX::SimpleMath::Vector2 position) {
	am->SetGroupSpatialPosition(channelGroupName, position);
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

void Audible::StopSound(std::string soundName) {
	for (auto channelIt = channels.begin(); channelIt != channels.end(); ++channelIt) {
		if (channelIt->second == soundName) {
			am->Stop(channelIt->first);

			if (channelIt->first == dangerScaleChannel) {
				dangerScaleChannel = -1;
				dangerScaleChannelVolumeMax = 100;
			}

			channels.erase(channelIt->first);
			break;
		}
	}
}


void Audible::PlaySoundsOnEvent(SoundEvent se) {
	for (auto sound : sounds) {
		if (std::find(sound.playEvents.begin(), sound.playEvents.end(), (int)se) != sound.playEvents.end()) {
			PlaySound(sound);
		}
	}
}

void Audible::StopSoundsOnEvent(SoundEvent se) {
	for (auto sound : sounds) {
		if (std::find(sound.stopEvents.begin(), sound.stopEvents.end(), (int)se) != sound.stopEvents.end()) {
			StopSound(sound.name);
		}
	}
}

void Audible::HandleSoundsOnEvent(SoundEvent se) {
	StopSoundsOnEvent(se);
	PlaySoundsOnEvent(se);
}



void Audible::HandleMessage(Message* e) {

}

void Audible::Deserialize(nlohmann::json j, GameObject* componentOwner)
{
	this->componentOwner = componentOwner;
	am = g_AudioManager.get();
	sounds = {};

	if (j.contains("sounds")) {
		for (auto it = std::begin(j["sounds"]); it != std::end(j["sounds"]); it++) {
			SoundInfo soundInfo;
			auto sound = *it;

			if (sound.contains("name")) 
				soundInfo.name = sound["name"];
			if (sound.contains("loop")) 
				soundInfo.loop = sound["loop"];
			if (sound.contains("compressed")) 
				soundInfo.compressed = sound["compressed"];
			if (sound.contains("isMusic"))
				soundInfo.isMusic = sound["isMusic"];
			if (sound.contains("volume")) 
				soundInfo.volume = sound["volume"];
			if (sound.contains("scaleVolumeWithDanger")) 
				soundInfo.scaleVolumeWithDanger = sound["scaleVolumeWithDanger"];
			if (sound.contains("startTime")) 
				soundInfo.startTime = sound["startTime"];
			if (sound.contains("pitchRange")) {
				soundInfo.pitchRange[0] = sound["pitchRange"][0];
				soundInfo.pitchRange[1] = sound["pitchRange"][1];
			}

			if (sound.contains("playEvents"))
				soundInfo.playEvents.insert(soundInfo.playEvents.begin(), std::begin(sound["playEvents"]), std::end(sound["playEvents"]));
			if (sound.contains("stopEvents"))
				soundInfo.stopEvents.insert(soundInfo.stopEvents.begin(), std::begin(sound["stopEvents"]), std::end(sound["stopEvents"]));


			sounds.push_back(soundInfo);
		}
	}

	if (j.contains("positionless")) 
		positionless = j["positionless"];
	if (j.contains("generateRandomTime"))
		generateRandomTime = j["generateRandomTime"];

}
