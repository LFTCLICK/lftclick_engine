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
#include "FMODResultStrings.h"
#include "SimpleMath.h"

#define MUSIC_MASTER_CHANNEL_GROUP std::string("musicMasterChannelGroup")
#define MENU_MUSIC_MASTER_CHANNEL_GROUP std::string("menuMusicMasterChannelGroup")
#define SFX_MASTER_CHANNEL_GROUP std::string("sfxMasterChannelGroup")
#define BUTTON_CLICK_MASTER_CHANNEL_GROUP std::string("buttonClickMasterChannelGroup")

#define VOLUME_DIV 100.f
#define POSITION_DIV 250.f

#endif

struct FMODEngine {

    FMODEngine();
    ~FMODEngine();
    FMOD_RESULT Update();
    bool ChannelIsPlaying(FMOD::Channel* channel);
    int GenerateChannelID();
    int nextID = 0;

    FMOD::System* system;


    std::map<int, FMOD::Channel*> channels;
    std::map<std::string, FMOD::ChannelGroup*> channelGroups;

    std::map<std::string, FMOD::Sound*> sounds;
    std::map<std::string, FMOD::SoundGroup*> soundGroups;
};

class AudioManager
{
public:
    AudioManager();
    ~AudioManager();

    AudioManager(AudioManager const&) = delete;
    void operator=(AudioManager const&) = delete;
    void Init();
    void Update();
    //void Term();


    // Functions for loading/unloading sounds and playing them.
    // Playing a sound creates a channel, and passing a group name will add the channel to the channel group.

    void LoadSound(std::string name, bool loop = false, bool compressed = true, bool is3D = true);
    void UnloadSound(std::string name);
    int PlaySound(std::string name, std::string channelGroupName, float volume = 100.f, float pitch = 1.f, float startTime = 0, bool startPaused = false);
    int PlaySound(std::string name, float volume = 100.f, float pitch = 1.f, float startTime = 0, bool startPaused = false);
    void SetPlayerSpatialPosition(DirectX::SimpleMath::Vector2 _position, DirectX::SimpleMath::Vector2 _velocity = { 0, 0 });


    // Functions for managing channels

    void Pause(int channelID);
    void Unpause(int channelID);
    void SetPaused(int channelID, bool paused);
    bool IsPaused(int channelID);
    float GetVolume(int channelID);
    void SetVolume(int channelID, float volume);
    void SetMute(int channelID, bool muted);
    void Mute(int channelID);
    void Unmute(int channelID);
    bool IsMuted(int channelID);
    DirectX::SimpleMath::Vector2 GetSpatialPosition(int channelID);
    void SetSpatialPosition(int channelID, float posX, float posY, float velX = 0, float velY = 0);
    void SetSpatialPosition(int channelID, DirectX::SimpleMath::Vector2 position, DirectX::SimpleMath::Vector2 velocity = { 0, 0 });
    float GetPitch(int channelID);
    void SetPitch(int channelID, float pitch);
    float GetFrequency(int channelID);
    void SetFrequency(int channelID, float frequency);
    bool IsPlaying(int channelID);
    void Stop(int channelID);


    // Functions for loading/unloading channel groups and adding to them.
    // Currently only one level of channel groups is planned for.

    void LoadChannelGroup(std::string name, FMOD_MODE mode = FMOD_3D, std::string masterChannelGroup = SFX_MASTER_CHANNEL_GROUP);
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
    void SetGroupMute(std::string channelGroupName, bool muted);
    void MuteGroup(std::string channelGroupName);
    void UnmuteGroup(std::string channelGroupName);
    bool IsGroupMuted(std::string channelGroupName);
    void SetMasterMute(bool muted);
    void ToggleMuteMusic();
    void MuteMaster();
    void UnmuteMaster();
    bool IsMasterMuted();
    DirectX::SimpleMath::Vector2 GetGroupSpatialPosition(std::string channelGroupName);
    void SetGroupSpatialPosition(std::string channelGroupName, float posX, float posY, float velX = 0, float velY = 0);
    void SetGroupSpatialPosition(std::string channelGroupName, DirectX::SimpleMath::Vector2 position, DirectX::SimpleMath::Vector2 velocity = { 0, 0 });
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

    std::string GenerateUniqueChannelGroupID();

private:
    FMODEngine* engine;
    long int nextChannelGroupID;
    void CheckResult(std::string functionName, FMOD_RESULT e);
};

extern std::unique_ptr<AudioManager> g_AudioManager;