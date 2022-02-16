// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	AudioManager.h
// Author			:	Chris Fitch
// Creation Date	:	2022/01/27
// Purpose			:	FMOD Audio Handling Header
// History			:
// ---------------------------------------------------------------------------
#pragma once

#ifndef _AUDIO_MANAGER_H_
#define _AUDIO_MANAGER_H_

#include "fmod.hpp"
#include <string>
#include <map>
#include <vector>
#include <iterator>
#include <math.h>
#include <iostream>
#include "Vector2D.h"
#include "FMODResultStrings.h"

#define VOLUME_DIV 100.f

#endif

struct FMODEngine {
    FMODEngine();
    ~FMODEngine();
    void Update();
    bool ChannelIsPlaying(FMOD::Channel* channel);

    FMOD::System* system;

    std::map<int, FMOD::Channel*> channels;
    std::map<std::string, FMOD::ChannelGroup*> channelGroups;

    std::map<std::string, FMOD::Sound*> sounds;
    std::map<std::string, FMOD::SoundGroup*> soundGroups;
};

class AudioManager
{
public:
    static AudioManager& getInstance()
    {
        static AudioManager instance;
        return instance;
    }
    AudioManager(AudioManager const&) = delete;
    void operator=(AudioManager const&) = delete;
    void Init();
    void Update();
    void Term();


    // Functions for loading/unloading sounds and playing them.
    // Playing a sound creates a channel, and passing a group name will add the channel to the channel group.

    void LoadSound(std::string name, bool loop = false, bool compressed = true);
    void UnloadSound(std::string name);
    int PlaySound(std::string name, std::string channelGroupName, float volume = 100.f, float x = 0, float y = 0, bool startPaused = false);
    int PlaySound(std::string name, std::string channelGroupName, float volume, Vector2D position, bool startPaused = false);
    int PlaySound(std::string name, float volume = 100.f, float x = 0, float y = 0, bool startPaused = false);
    int PlaySound(std::string name, float volume, Vector2D position, bool startPaused = false);


    // Functions for managing channels

    void Pause(int channelID);
    void Unpause(int channelID);
    void SetPaused(int channelID, bool paused);
    bool IsPaused(int channelID);
    float GetVolume(int channelID);
    void SetVolume(int channelID, float volume);
    void Mute(int channelID) { SetVolume(channelID, 0); }
    Vector2D GetPosition(int channelID);
    void SetPosition(int channelID, float x, float y);
    void SetPosition(int channelID, Vector2D position);
    float GetPitch(int channelID);
    void SetPitch(int channelID, float pitch);
    float GetFrequency(int channelID);
    void SetFrequency(int channelID, float frequency);
    bool IsPlaying(int channelID);
    void Stop(int channelID);


    // Functions for loading/unloading channel groups and adding to them.
    // Currently only one level of channel groups is planned for.

    void LoadChannelGroup(std::string name);
    void UnloadChannelGroup(std::string name);
    void AddToChannelGroup(std::string channelGroupName, int channelID);
    void AddToChannelGroup(std::string channelGroupName, FMOD::Channel* channel);
    void AddToChannelGroup(FMOD::ChannelGroup* channelGroup, int channelID);


    // Nearly the same channel management functions, but for channel groups.

    void PauseGroup(std::string channelGroupName);
    void UnpauseGroup(std::string channelGroupName);
    void SetGroupPaused(std::string channelGroupName, bool paused);
    bool IsGroupPaused(std::string channelGroupName);
    float GetGroupVolume(std::string channelGroupName);
    void SetGroupVolume(std::string channelGroupName, float volume);
    void MuteGroup(std::string channelGroupName) { SetGroupVolume(channelGroupName, 0); }
    Vector2D GetGroupPosition(std::string channelGroupName);
    void SetGroupPosition(std::string channelGroupName, float x, float y);
    void SetGroupPosition(std::string channelGroupName, Vector2D position);
    float GetGroupPitch(std::string channelGroupName);
    void SetGroupPitch(std::string channelGroupName, float pitch);
    bool IsGroupPlaying(std::string channelGroupName);
    void StopGroup(std::string channelGroupName);


    // Functions for loading/unloading sound groups and adding to them.
    // Currently only one level of sound groups is planned for.

    void LoadSoundGroup(std::string name);
    void UnloadSoundGroup(std::string name);
    void AddToSoundGroup(std::string soundGroupName, std::string soundName);
    void AddToSoundGroup(std::string soundGroupName, FMOD::Sound* sound);
    void AddToSoundGroup(FMOD::SoundGroup* soundGroup, std::string soundName);

private:
    AudioManager();
    FMODEngine* engine;
    void CheckResult(std::string functionName, FMOD_RESULT e);
};