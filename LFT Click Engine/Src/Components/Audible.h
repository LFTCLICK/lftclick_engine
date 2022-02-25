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
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>

#define ON_START 1000
#define ON_MOVE 1001
#define ON_HALT 1002

struct SoundInfo {
	std::string name;
	bool loop = false;
	bool compressed = true;
	int pitchRange[2] = { 1, 1 };
	float volume = 100.f;
	std::vector<int> playEvents;
	std::vector<int> stopEvents;
};

class Audible : public Component
{
public:
	// Inherited via Component
	virtual void Start() override;
	virtual void Update() override;
	virtual int getCompId() override { return ComponentType::AUDIBLE; };
	virtual Component* Clone(GameObject* newParent);
	virtual void HandleMessage(Message* e) override;
	virtual void Deserialize(nlohmann::json j, GameObject* parent) override;

	Audible() : parent(nullptr), am(&AudioManager::getInstance()), sounds({}), position({ 0.0f }), positionOffset({ 0.0f }) {}
	~Audible();

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


	DirectX::SimpleMath::Vector2 positionOffset;

protected:
	GameObject* parent;
	AudioManager* am;
	DirectX::SimpleMath::Vector2 oldPosition;
	DirectX::SimpleMath::Vector2 position;
	bool wasMoving;
	std::string channelGroupName;

	std::vector<SoundInfo> sounds;
	std::map<int, std::string> channels;
};
