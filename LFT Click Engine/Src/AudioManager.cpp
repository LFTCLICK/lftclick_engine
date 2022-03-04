// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	AudioManager.cpp
// Author			:	Chris Fitch
// Creation Date	:	2022/01/27
// Purpose			:	FMOD Audio Handling 
// History			:
// ---------------------------------------------------------------------------

#include "pch.h"
#include "AudioManager.h"



// FMOD Engine constructor.
FMODEngine::FMODEngine() : system(nullptr) {
	FMOD::System_Create(&system);
	void* extraDriverData = nullptr;
	system->init(2047, FMOD_INIT_NORMAL, extraDriverData);
}

// FMOD Engine destructor.
FMODEngine::~FMODEngine() {
	system->close();
	system->release();
}

// Update the FMOD engine. Normally called by the audio manager. Should run every frame.
void FMODEngine::Update() {
	//for (auto current = channels.begin(), end = channels.end(); current != end; ++current)
		//if (ChannelIsPlaying(current->second)) current++;
		//else current = channels.erase(current);
			
	system->update();
}

// Returns whether the given channel is currently playing.
bool FMODEngine::ChannelIsPlaying(FMOD::Channel* channel) {
	bool isPlaying = true;
	channel->isPlaying(&isPlaying);
	return isPlaying;
}

int FMODEngine::GenerateChannelID() {
	return nextID++;
}


// Audio Manager constructor.
AudioManager::AudioManager() : engine(nullptr), nextChannelGroupID(0) {}


// Initializes the manager and FMOD engine. Should run only on game initialization.
void AudioManager::Init() {
	engine = new FMODEngine;
}

// Updates the manager and FMOD engine. Should run every frame.
void AudioManager::Update() {
	engine->Update();
}

// Terminates the manager and FMOD engine. Should run only on game termination.
void AudioManager::Term() {
	delete engine;
}

// Loads a sound into FMOD's memory and into the internal sounds map.
void AudioManager::LoadSound(std::string name, bool loop, bool compressed) {
	if (engine->sounds.find(name) == engine->sounds.end()) {
		FMOD_MODE fmod_mode = 
			FMOD_3D | 
			(compressed ? FMOD_CREATECOMPRESSEDSAMPLE : FMOD_CREATESAMPLE) | 
			(loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
		FMOD::Sound* sound = nullptr;
		CheckResult(__func__, engine->system->createSound(name.c_str(), fmod_mode, nullptr, &sound));
		engine->sounds[name] = sound;
	}
}

// Unloads a sound from FMOD's memory.
void AudioManager::UnloadSound(std::string name) {
	auto sound = engine->sounds.find(name);
	if (sound != engine->sounds.end()) {
		CheckResult(__func__, sound->second->release());
		engine->sounds.erase(sound);
	}
}

// Plays a sound. Will load the sound using the "Load" function if it
// has not already been loaded.
// 
// The volume value given is divided by 100 when being sent to FMOD,
// as FMOD's volume operates between 0 and 1.
int AudioManager::PlaySound(std::string name, std::string channelGroupName, float volume, float x, float y, float pitch, bool startPaused) {
	auto sound = engine->sounds.find(name);
	if (sound == engine->sounds.end()) {
		std::cout << name << " was not preloaded, loading from source instead." << std::endl;
		AudioManager::LoadSound(name);
		sound = engine->sounds.find(name);
	}

	if (sound != engine->sounds.end()) {
		FMOD::Channel* channel = nullptr;
		auto channelGroupIt = engine->channelGroups.find(channelGroupName);
		FMOD::ChannelGroup* channelGroup = (channelGroupIt == engine->channelGroups.end() ? nullptr : channelGroupIt->second);
		CheckResult(__func__, engine->system->playSound(sound->second, channelGroup, true, &channel));

		FMOD_VECTOR position = { x, y, 0 };
		CheckResult(__func__, channel->set3DAttributes(&position, nullptr));
		CheckResult(__func__, channel->setVolume(volume / VOLUME_DIV));
		if (pitch != 1.f) CheckResult(__func__, channel->setPitch(pitch));
		CheckResult(__func__, channel->setPaused(startPaused));

		int channelIndex = engine->GenerateChannelID();
		engine->channels[channelIndex] = channel;
		return channelIndex;
	}
	else {
		std::cout << name << " could not be loaded.";
		return -1;
	}
}
int AudioManager::PlaySound(std::string name, std::string channelGroupName, float volume, DirectX::SimpleMath::Vector2 position, float pitch, bool startPaused) {
	return AudioManager::PlaySound(name, channelGroupName, volume, position.x, position.y, pitch);
}
int AudioManager::PlaySound(std::string name, float volume, DirectX::SimpleMath::Vector2 position, float pitch, bool startPaused) {
	return AudioManager::PlaySound(name, "", volume, position.x, position.y, pitch);
}
int AudioManager::PlaySound(std::string name, float volume, float x, float y, float pitch, bool startPaused) {
	return AudioManager::PlaySound(name, "", volume, x, y, pitch);
}

// Pauses a channel.
void AudioManager::Pause(int channelID) {
	AudioManager::SetPaused(channelID, true);
}

// Unpauses a channel.
void AudioManager::Unpause(int channelID) {
	AudioManager::SetPaused(channelID, false);
}

// Sets whether a channel is paused.
void AudioManager::SetPaused(int channelID, bool paused) {
	auto channel = engine->channels.find(channelID);
	if (channel != engine->channels.end())
		CheckResult(__func__, channel->second->setPaused(paused));
}

// Returns whether a channel group is paused.
bool AudioManager::IsPaused(int channelID) {
	bool paused = false;
	auto channel = engine->channels.find(channelID);
	if (channel != engine->channels.end())
		CheckResult(__func__, channel->second->getPaused(&paused));
	return paused;
}

// Gets the channel's volume between 1 and 100.
float AudioManager::GetVolume(int channelID) {
	float volume = 0;
	auto channel = engine->channels.find(channelID);
	if (channel != engine->channels.end()) {
		CheckResult(__func__, channel->second->getVolume(&volume));
	}
	return volume * VOLUME_DIV;
}

// Sets volume on a channel between 1 and 100.
// The volume value given is divided by 100 when being sent to FMOD,
// as FMOD's volume operates between 0 and 1.
void AudioManager::SetVolume(int channelID, float volume) {
	auto channel = engine->channels.find(channelID);
	if (channel != engine->channels.end())
		CheckResult(__func__, channel->second->setVolume(volume / VOLUME_DIV));
}

// Gets the channel's world position.
DirectX::SimpleMath::Vector2 AudioManager::GetPosition(int channelID) {
	FMOD_VECTOR position = { 0, 0, 0 }, velocity = { 0, 0, 0 };
	auto channel = engine->channels.find(channelID);
	if (channel != engine->channels.end()) {
		CheckResult(__func__, channel->second->get3DAttributes(&position, &velocity));
	}
	return DirectX::SimpleMath::Vector2(position.x, position.y);
}

// Sets the channel's world position.
void AudioManager::SetPosition(int channelID, float x, float y) {
	auto channel = engine->channels.find(channelID);
	if (channel != engine->channels.end()) {
		FMOD_VECTOR position = {x, y, 0};
		CheckResult(__func__, channel->second->set3DAttributes(&position, NULL));
	}
}
void AudioManager::SetPosition(int channelID, DirectX::SimpleMath::Vector2 position) {
	AudioManager::SetPosition(channelID, position.x, position.y);
}

// Gets the channel's pitch, with 0.5 being an octave down and 2 being an octave up.
float AudioManager::GetPitch(int channelID) {
	float pitch = 0;
	auto channel = engine->channels.find(channelID);
	if (channel != engine->channels.end()) {
		CheckResult(__func__, channel->second->getPitch(&pitch));
	}
	return pitch;
}

// Sets the channel's pitch, with 0.5 being an octave down and 2 being an octave up.
void AudioManager::SetPitch(int channelID, float pitch) {
	auto channel = engine->channels.find(channelID);
	if (channel != engine->channels.end()) {
		CheckResult(__func__, channel->second->setPitch(pitch));
	}
}

// Gets the channel's frequency (aka speed).
float AudioManager::GetFrequency(int channelID) {
	float frequency = 0;
	auto channel = engine->channels.find(channelID);
	if (channel != engine->channels.end()) {
		CheckResult(__func__, channel->second->getFrequency(&frequency));
	}
	return frequency;
}

// Sets the channel's frequency (aka speed). Shouldn't be applied without first using GetFrequency.
// Negative value makes playback go backwards, but sample must have been loaded non-compressed.
void AudioManager::SetFrequency(int channelID, float frequency) {
	auto channel = engine->channels.find(channelID);
	if (channel != engine->channels.end()) {
		CheckResult(__func__, channel->second->setFrequency(frequency));
	}
}

// Returns whether the channel is currently playing.
bool AudioManager::IsPlaying(int channelID) {
	bool isPlaying = false;
	auto channel = engine->channels.find(channelID);
	if (channel != engine->channels.end()) 
		CheckResult(__func__, channel->second->isPlaying(&isPlaying));
	return isPlaying;
}

// Stops the channel and frees up the memory.
void AudioManager::Stop(int channelID) {
	auto channel = engine->channels.find(channelID);
	if (channel != engine->channels.end())
		CheckResult(__func__, channel->second->stop());
}



// Pauses a channel group
void AudioManager::PauseGroup(std::string channelGroupName) {
	AudioManager::SetGroupPaused(channelGroupName, false);
}

// Unpauses a channel group
void AudioManager::UnpauseGroup(std::string channelGroupName) {
	AudioManager::SetGroupPaused(channelGroupName, false);
}

// Sets whether a channel group is paused.
void AudioManager::SetGroupPaused(std::string channelGroupName, bool paused) {
	auto channelGroup = engine->channelGroups.find(channelGroupName);
	if (channelGroup != engine->channelGroups.end())
		CheckResult(__func__, channelGroup->second->setPaused(paused));
}

// Returns whether a channel group is paused.
bool AudioManager::IsGroupPaused(std::string channelGroupName) {
	bool paused = false;
	auto channelGroup = engine->channelGroups.find(channelGroupName);
	if (channelGroup != engine->channelGroups.end())
		CheckResult(__func__, channelGroup->second->getPaused(&paused));
	return paused;
}

// Returns the channel group's volume between 1 and 100.
float AudioManager::GetGroupVolume(std::string channelGroupName) {
	float volume = 0;
	auto channelGroup = engine->channelGroups.find(channelGroupName);
	if (channelGroup != engine->channelGroups.end()) {
		CheckResult(__func__, channelGroup->second->getVolume(&volume));
	}
	return volume * VOLUME_DIV;
}

// Sets volume on a channel group between 1 and 100.
// The volume value given is divided by 100 when being sent to FMOD,
// as FMOD's volume operates between 0 and 1.
void AudioManager::SetGroupVolume(std::string channelGroupName, float volume) {
	auto channelGroup = engine->channelGroups.find(channelGroupName);
	if (channelGroup != engine->channelGroups.end())
		CheckResult(__func__, channelGroup->second->setVolume(volume / VOLUME_DIV));
}

// Returns the channel group's world position.
DirectX::SimpleMath::Vector2 AudioManager::GetGroupPosition(std::string channelGroupName) {
	FMOD_VECTOR position = { 0, 0, 0 }, velocity = { 0, 0, 0 };
	auto channelGroup = engine->channelGroups.find(channelGroupName);
	if (channelGroup != engine->channelGroups.end()) {
		CheckResult(__func__, channelGroup->second->get3DAttributes(&position, &velocity));
	}
	return DirectX::SimpleMath::Vector2(position.x, position.y);
}

// Sets the channel group's world position.
void AudioManager::SetGroupPosition(std::string channelGroupName, float x, float y) {
	auto channelGroup = engine->channelGroups.find(channelGroupName);
	if (channelGroup != engine->channelGroups.end()) {
		FMOD_VECTOR position = { x, y, 0 };
		CheckResult(__func__, channelGroup->second->set3DAttributes(&position, NULL));
	}
}
void AudioManager::SetGroupPosition(std::string channelGroupName, DirectX::SimpleMath::Vector2 position) {
	AudioManager::SetGroupPosition(channelGroupName, position.x, position.y);
}

// Returns the channel group's pitch, with 0.5 being an octave down and 2 being an octave up.
float AudioManager::GetGroupPitch(std::string channelGroupName) {
	float pitch = 0;
	auto channelGroup = engine->channelGroups.find(channelGroupName);
	if (channelGroup != engine->channelGroups.end()) {
		CheckResult(__func__, channelGroup->second->getPitch(&pitch));
	}
	return pitch;
}

// Sets the channel group's pitch, with 0.5 being an octave down and 2 being an octave up.
void AudioManager::SetGroupPitch(std::string channelGroupName, float pitch) {
	auto channelGroup = engine->channelGroups.find(channelGroupName);
	if (channelGroup != engine->channelGroups.end()) {
		CheckResult(__func__, channelGroup->second->setPitch(pitch));
	}
}

// Returns whether the channel group is currently playing.
bool AudioManager::IsGroupPlaying(std::string channelGroupName) {
	bool isPlaying = false;
	auto channelGroup = engine->channelGroups.find(channelGroupName);
	if (channelGroup != engine->channelGroups.end())
		CheckResult(__func__, channelGroup->second->isPlaying(&isPlaying));
	return isPlaying;
}

// Stops the channel group and frees up the memory.
void AudioManager::StopGroup(std::string channelGroupName) {
	auto channelGroup = engine->channelGroups.find(channelGroupName);
	if (channelGroup != engine->channelGroups.end())
		CheckResult(__func__, channelGroup->second->stop());
}



void AudioManager::LoadChannelGroup(std::string name) {
	if (engine->channelGroups.find(name) == engine->channelGroups.end()) {
		FMOD::ChannelGroup* channelGroup;
		CheckResult(__func__, engine->system->createChannelGroup(name.c_str(), &channelGroup));
		engine->channelGroups[name] = channelGroup;
	}
}

void AudioManager::UnloadChannelGroup(std::string name) {
	auto channelGroup = engine->channelGroups.find(name);
	if (channelGroup != engine->channelGroups.end()) {
		CheckResult(__func__, channelGroup->second->release());
	}
}

void AudioManager::AddToChannelGroup(std::string channelGroupName, int channelID) {
	auto channel = engine->channels.find(channelID);
	if (channel != engine->channels.end())
		AddToChannelGroup(channelGroupName, channel->second);
}
void AudioManager::AddToChannelGroup(std::string channelGroupName, FMOD::Channel* channel) {
	auto channelGroup = engine->channelGroups.find(channelGroupName);
	if (channelGroup != engine->channelGroups.end())
		channel->setChannelGroup(channelGroup->second);
}
void AudioManager::AddToChannelGroup(FMOD::ChannelGroup* channelGroup, int channelID) {
	auto channel = engine->channels.find(channelID);
	if (channel != engine->channels.end())
		channel->second->setChannelGroup(channelGroup);
}



void AudioManager::LoadSoundGroup(std::string name) {
	auto soundGroup = engine->soundGroups.find(name);
	if (soundGroup == engine->soundGroups.end()) {
		CheckResult(__func__, engine->system->createSoundGroup(name.c_str(), &(soundGroup->second)));
	}
}

void AudioManager::UnloadSoundGroup(std::string name) {
	auto soundGroup = engine->soundGroups.find(name);
	if (soundGroup != engine->soundGroups.end()) {
		CheckResult(__func__, soundGroup->second->release());
	}
}

void AudioManager::AddToSoundGroup(std::string soundGroupName, std::string soundName) {
	auto sound = engine->sounds.find(soundName);
	if (sound != engine->sounds.end())
		AddToSoundGroup(soundGroupName, sound->second);
}
void AudioManager::AddToSoundGroup(std::string soundGroupName, FMOD::Sound* sound) {
	auto soundGroup = engine->soundGroups.find(soundGroupName);
	if (soundGroup != engine->soundGroups.end())
		sound->setSoundGroup(soundGroup->second);
}
void AudioManager::AddToSoundGroup(FMOD::SoundGroup* soundGroup, std::string soundName) {
	auto sound = engine->sounds.find(soundName);
	if (sound != engine->sounds.end())
		sound->second->setSoundGroup(soundGroup);
}


std::string AudioManager::GenerateUniqueChannelGroupID() {
	return std::to_string(nextChannelGroupID++);
}


// Prints the result of an FMOD function if it's anything but FMOD_OK
void AudioManager::CheckResult(std::string functionName, FMOD_RESULT e) {
	if (e != FMOD_OK && !(e == FMOD_ERR_INVALID_HANDLE && (functionName == "IsPlaying" || functionName == "IsPaused"))) {
		auto FMODResultString = FMOD_RESULT_STRINGS.find(e);
		if (FMODResultString == FMOD_RESULT_STRINGS.end())
			std::cout << functionName << " failed with FMOD_RESULT enum " << e << std::endl;
		else
			std::cout << functionName << " failed with FMOD_RESULT type " << FMODResultString->second << std::endl;
	}
}