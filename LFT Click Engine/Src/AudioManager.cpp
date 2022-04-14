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
	FMOD::ChannelGroup *musicMasterChannelGroup, *menuMusicMasterChannelGroup, *sfxMasterChannelGroup, *masterChannelGroup;

	void* extraDriverData = nullptr;
	system->init(2047, FMOD_INIT_NORMAL, extraDriverData);
	system->getMasterChannelGroup(&masterChannelGroup);

	system->set3DSettings(1.f, 1000000.f, 1.f);

	system->createChannelGroup(MUSIC_MASTER_CHANNEL_GROUP.c_str(), &musicMasterChannelGroup);
	musicMasterChannelGroup->setMode(FMOD_3D | FMOD_CREATESAMPLE | FMOD_LOOP_NORMAL);
	channelGroups[MUSIC_MASTER_CHANNEL_GROUP] = musicMasterChannelGroup;

	system->createChannelGroup(MENU_MUSIC_MASTER_CHANNEL_GROUP.c_str(), &menuMusicMasterChannelGroup);
	menuMusicMasterChannelGroup->setMode(FMOD_3D | FMOD_CREATESAMPLE | FMOD_LOOP_NORMAL);
	channelGroups[MENU_MUSIC_MASTER_CHANNEL_GROUP] = menuMusicMasterChannelGroup;

	system->createChannelGroup(SFX_MASTER_CHANNEL_GROUP.c_str(), &sfxMasterChannelGroup);
	sfxMasterChannelGroup->setMode(FMOD_3D | FMOD_CREATECOMPRESSEDSAMPLE | FMOD_LOOP_OFF);
	channelGroups[SFX_MASTER_CHANNEL_GROUP] = sfxMasterChannelGroup;
	sfxMasterChannelGroup->set3DAttributes({ 0 }, { 0 });
}

// FMOD Engine destructor.
FMODEngine::~FMODEngine() {
	system->close();
	system->release();
}

// Update the FMOD engine. Normally called by the audio manager. Should run every frame.
FMOD_RESULT FMODEngine::Update() {
	//for (auto current = channels.begin(), end = channels.end(); current != end; ++current)
		//if (ChannelIsPlaying(current->second)) current++;
		//else current = channels.erase(current);
	return system->update();
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
	CheckResult(__func__, engine->Update());
	/*for (auto channel : engine->channels) {
		float aud;
		channel.second->getAudibility(&aud);
		std::cout << aud << " ";
	}
	std::cout << std::endl;*/
}

//// Terminates the manager and FMOD engine. Should run only on game termination.
//void AudioManager::Term() {
//	delete engine;
//}

AudioManager::~AudioManager()
{
	delete engine;
}

// Loads a sound into FMOD's memory and into the internal sounds map.
void AudioManager::LoadSound(std::string name, bool loop, bool compressed, bool is3D) {
	if (engine->sounds.find(name) == engine->sounds.end()) {
		FMOD_MODE fmod_mode = 
			(is3D ? FMOD_3D : FMOD_2D) |
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
int AudioManager::PlaySound(std::string name, std::string channelGroupName, float volume, float pitch, float startTime, bool startPaused) {
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

		float vol;
		CheckResult(__func__, channel->setVolume(volume / VOLUME_DIV));

		if (pitch != 1.f) CheckResult(__func__, channel->setPitch(pitch));
		if (startTime != 0) CheckResult(__func__, channel->setPosition((int)(startTime * 1000), FMOD_TIMEUNIT_MS));
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
int AudioManager::PlaySound(std::string name, float volume, float pitch, float startTime, bool startPaused) {
	return AudioManager::PlaySound(name, "", volume, pitch, startTime, startPaused);
}

// Sets the position of orientation for spatial sounds.
void AudioManager::SetPlayerSpatialPosition(DirectX::SimpleMath::Vector2 _position, DirectX::SimpleMath::Vector2 _velocity) {
	DirectX::SimpleMath::Vector2 pos = _position, vel = _velocity;
	FMOD_VECTOR position = { pos.x / POSITION_DIV, pos.y / POSITION_DIV, 0 };
	FMOD_VECTOR velocity = { vel.x / POSITION_DIV, vel.y / POSITION_DIV, 0 };
	FMOD_VECTOR forward = { 0, 0, 1 };
	FMOD_VECTOR up = { 0, 1, 0 };
	//FMOD_VECTOR x1, x2, x3, x4;
	engine->system->set3DListenerAttributes(0, &position, &velocity, &forward, &up);
	//engine->system->get3DListenerAttributes(0, &x1, &x2, &x3, &x4);

	//std::cout << "player: " << x1.x << " " << x1.y << " " << x1.z << "    " << x2.x << " " << x2.y << " " << x2.z << "    " << x3.x << " " << x3.y << " " << x3.z << "    " << x4.x << " " << x4.y << " " << x4.z << "    " << std::endl;
	
	SetGroupSpatialPosition(MUSIC_MASTER_CHANNEL_GROUP, _position, _velocity);
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
DirectX::SimpleMath::Vector2 AudioManager::GetSpatialPosition(int channelID) {
	FMOD_VECTOR position = { 0, 0, 0 }, velocity = { 0, 0, 0 };
	auto channel = engine->channels.find(channelID);
	if (channel != engine->channels.end()) {
		CheckResult(__func__, channel->second->get3DAttributes(&position, &velocity));
	}
	return DirectX::SimpleMath::Vector2(position.x * POSITION_DIV, position.y * POSITION_DIV);
}

// Sets the channel's world position.
void AudioManager::SetSpatialPosition(int channelID, float posX, float posY, float velX, float velY) {
	auto channel = engine->channels.find(channelID);
	if (channel != engine->channels.end()) {
		/*
		FMOD::ChannelGroup *channelGroup;
		int length = 50;
		char* name = new char[50];
		channel->second->getChannelGroup(&channelGroup);
		channelGroup->getName(name, length);
		*/

		FMOD_VECTOR position = { posX / POSITION_DIV, posY / POSITION_DIV, 0 };
		FMOD_VECTOR velocity = { velX / POSITION_DIV, velY / POSITION_DIV, 0 };
		CheckResult(__func__, channel->second->set3DAttributes(&position, &velocity));
	}
}
void AudioManager::SetSpatialPosition(int channelID, DirectX::SimpleMath::Vector2 position, DirectX::SimpleMath::Vector2 velocity) {
	AudioManager::SetSpatialPosition(channelID, position.x, position.y, velocity.x, velocity.y);
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
	AudioManager::SetGroupPaused(channelGroupName, true);
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
DirectX::SimpleMath::Vector2 AudioManager::GetGroupSpatialPosition(std::string channelGroupName) {
	FMOD_VECTOR position = { 0, 0, 0 }, velocity = { 0, 0, 0 };
	auto channelGroup = engine->channelGroups.find(channelGroupName);
	if (channelGroup != engine->channelGroups.end()) {
		CheckResult(__func__, channelGroup->second->get3DAttributes(&position, &velocity));
	}
	return DirectX::SimpleMath::Vector2(position.x * POSITION_DIV, position.y * POSITION_DIV);
}

// Sets the channel group's world position.
void AudioManager::SetGroupSpatialPosition(std::string channelGroupName, float posX, float posY, float velX, float velY) {
	auto channelGroup = engine->channelGroups.find(channelGroupName);
	if (channelGroup != engine->channelGroups.end()) {
		FMOD_VECTOR position = { posX / POSITION_DIV, posY / POSITION_DIV, 0 };
		//if (channelGroupName != MUSIC_MASTER_CHANNEL_GROUP)
			//position = { position.x * 0.9f, position.y * 0.9f };
		FMOD_VECTOR velocity = { velX / POSITION_DIV, velY / POSITION_DIV, 0 };
		CheckResult(__func__, channelGroup->second->set3DAttributes(&position, &velocity));

		int numChannels;
		FMOD::Channel* currentChannel;
		CheckResult(__func__, channelGroup->second->getNumChannels(&numChannels));
		for (int i = 0; i < numChannels; i++) {
			CheckResult(__func__, channelGroup->second->getChannel(i, &currentChannel));
			CheckResult(__func__, currentChannel->set3DAttributes(&position, &velocity));
		}
	}
}
void AudioManager::SetGroupSpatialPosition(std::string channelGroupName, DirectX::SimpleMath::Vector2 position, DirectX::SimpleMath::Vector2 velocity) {
	AudioManager::SetGroupSpatialPosition(channelGroupName, position.x, position.y, velocity.x, velocity.y);
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



void AudioManager::LoadChannelGroup(std::string name, FMOD_MODE mode, std::string masterChannelGroup) {
	if (engine->channelGroups.find(name) == engine->channelGroups.end()) {
		FMOD::ChannelGroup* channelGroup;
		CheckResult(__func__, engine->system->createChannelGroup(name.c_str(), &channelGroup));
		channelGroup->setMode(mode);

		//engine->channelGroups[masterChannelGroup]->addGroup(channelGroup);
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