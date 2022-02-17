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
#include "../Vector2D.h"
#include <json.hpp>
#include "../AudioManager.h"
#include "../EventManager.h"
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

	Audible() : parent(nullptr), am(&AudioManager::getInstance()), sounds({}), position({ 0, 0 }), positionOffset({ 0, 0 }) {}
	Audible(json j, GameObject* parent);
	~Audible();

	void PlaySound(SoundInfo sound);
	void Unpause();
	void Pause();
	void SetPaused(bool paused);
	void SetVolume(float volume);
	void Mute();
	float GetVolume();
	Vector2D GetSpatialPosition();
	void SetSpatialPosition(float x, float y);
	void SetSpatialPosition(Vector2D position);
	float GetPitch();
	void SetPitch(float pitch);
	bool IsPlaying();
	void Stop();
	void StopSound(std::string soundName);

	virtual void HandleMessage(Message* e) override;

	Vector2D positionOffset;

protected:
	GameObject* parent;
	AudioManager* am;
	Vector2D oldPosition;
	Vector2D position;
	bool wasMoving;
	std::string channelGroupName;

	std::vector<SoundInfo> sounds;
	std::map<int, std::string> channels;
};