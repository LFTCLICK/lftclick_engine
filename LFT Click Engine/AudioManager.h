// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	FMODAudioManager.h
// Author			:	Chris Fitch
// Creation Date	:	2022/01/27
// Purpose			:	FMOD Audio Handling Header
// History			:
// ---------------------------------------------------------------------------
#pragma once

#ifndef _AUDIO_MANAGER_H_
#define _AUDIO_MANAGER_H_

#include "fmod_studio.hpp"
#include "fmod.hpp"
#include <string>
#include <map>
#include <vector>
#include <iterator>
#include <math.h>
#include <iostream>
#include "Vector2D.h"

#define VOLUME_DIV 100.f

#endif

struct FMODEngine {
    FMODEngine();
    ~FMODEngine();
    void Update();
    bool ChannelIsPlaying(FMOD::Channel* channel);

    FMOD::System* system;
    FMOD::Studio::System* studioSystem;

    std::map<int, FMOD::Channel*> channels;
    int newChannelID;
    std::map<std::string, FMOD::Sound*> sounds;

    std::map<std::string, FMOD::Studio::Bank*> banks;
    std::map<std::string, FMOD::Studio::EventInstance*> events;
};

class AudioManager
{
public:
	AudioManager();
	~AudioManager();
	void Init();
    void Update();

    void Load(std::string name, bool loop); 
    void LoadLoop(std::string name);
    void Unload(std::string name);
    int Play(std::string name, float x = 0, float y = 0, float volume = 100.f);
    int Play(std::string name, Vector2D position, float volume = 1.f);
    void Pause(int channelID);
    void Unpause(int channelID);
    void SetVolume(int channelID, float volume);
    void SetPosition(int channelID, float x, float y);
    void SetPosition(int channelID, Vector2D position);

    void LoadBank(std::string name);
    void LoadEvent(std::string name);

private:
    FMODEngine* engine;
    void SetPaused(int channelID, bool paused);

};