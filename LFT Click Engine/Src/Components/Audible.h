// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	Audible.h
// Author			:	Christian Fitch
// Creation Date	:	2022/02/10
// Purpose			:	Component to play sounds using audio manager
// History			:
// ---------------------------------------------------------------------------
#pragma once

#include "GameObject.h"
#include "Component.h"
#include "Transform.h"
#include <json.hpp>
#include "AudioManager.h"
#include "EventManager.h"
#include "FrameRateController.h"
#include "Helpers.h"
#include "GameManager.h"
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>

struct SoundInfo {
	std::string name;
	bool loop = false;
	bool compressed = true;
	bool isMusic = false;
	bool scaleVolumeWithDanger = false;
	float pitchRange[2] = { 1.f, 1.f };
	float volume = 100.f;
	float startTime = 0;
	std::vector<int> playEvents;
	std::vector<int> stopEvents;
};

class Audible : public Component
{
public:
	enum SoundEvent {
		AUDIO_ON_START = 1000,
		AUDIO_ON_MOVE,
		AUDIO_ON_HALT,
		AUDIO_ON_DAMAGE,
		AUDIO_ON_DEATH,
		AUDIO_ON_INTERACTING,
		AUDIO_ON_COLLECT,
		AUDIO_ON_RANDOM,
		AUDIO_ON_COLLECT_PART,
		AUDIO_ON_DOOR_BASH,
		AUDIO_ON_DOOR_BREAK,
		AUDIO_ON_DELETE = 2000,
	};
	// Inherited via Component
	virtual void Start() override;
	virtual void Update() override;
	virtual int getCompId() override { return ComponentType::AUDIBLE; };
	static int getStaticCompId() { return ComponentType::AUDIBLE; };
	virtual Component* Clone(GameObject* newParent);
	virtual void HandleMessage(Message* e) override;
	virtual void Deserialize(nlohmann::json j, GameObject* componentOwner) override;

	Audible() : 
		am(g_AudioManager.get()), 
		sounds({}), 
		frc(g_FrameRateController.get()), 
		positionless(false),
		dangerScaleChannel(-1),
		dangerScaleChannelVolumeMax(100),
		randomTime(-1.f),
		generateRandomTime(false)
	{}
	~Audible();

	void LoadSounds(std::vector<SoundInfo> newSounds);
	void PlaySound(SoundInfo sound);
	void Unpause();
	void Pause();
	void SetPaused(bool paused);
	void SetVolume(float volume);
	void Mute();
	float GetVolume();
	DirectX::SimpleMath::Vector2 GetPosition();
	void SetPosition(float x, float y);
	void SetPosition(DirectX::SimpleMath::Vector2 position);
	float GetPitch();
	void SetPitch(float pitch);
	bool IsPlaying();
	void Stop();
	void StopSound(std::string soundName);

	void PlaySoundsOnEvent(SoundEvent se);
	void StopSoundsOnEvent(SoundEvent se);
	void HandleSoundsOnEvent(SoundEvent se);

protected:
	AudioManager* am;
	FrameRateController* frc;
	Transform* myTransform;
	std::string channelGroupName;

	std::vector<SoundInfo> sounds;
	std::map<int, std::string> channels;

	bool positionless, generateRandomTime;
	int dangerScaleChannel;
	float dangerScaleChannelVolumeMax, randomTime;
};
