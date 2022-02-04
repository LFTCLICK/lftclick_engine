// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	FMODAudioManager.cpp
// Author			:	Chris Fitch
// Creation Date	:	2022/01/27
// Purpose			:	FMOD Audio Handling 
// History			:
// ---------------------------------------------------------------------------
#include "AudioManager.h"



// FMOD Engine constructor.
FMODEngine::FMODEngine() : system(NULL), studioSystem(NULL), newChannelID(0) {
	FMOD::Studio::System::create(&studioSystem);
	studioSystem->initialize(32, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, NULL);
	studioSystem->getCoreSystem(&system);
}

// FMOD Engine destructor.
FMODEngine::~FMODEngine() {
	studioSystem->unloadAll();
	studioSystem->release();
}

// Update the FMOD engine. Normally called by the audio manager. Should run every frame.
void FMODEngine::Update() {
	for (std::map<int, FMOD::Channel*>::iterator current = channels.begin(), end = channels.end(); current != end; ++current)
		if (ChannelIsPlaying(current->second)) current++;
		else current = channels.erase(current);
			
	studioSystem->update();
}

// Returns whether the given channel is currently playing.
bool FMODEngine::ChannelIsPlaying(FMOD::Channel* channel) {
	bool isPlaying = true;
	channel->isPlaying(&isPlaying);
	return isPlaying;
}


// Audio Manager constructor.
AudioManager::AudioManager() : engine(nullptr) {

}

// Audio Manager destructor.
AudioManager::~AudioManager() {

}


// Initializes the manager and FMOD engine. Should run only on game initialization.
void AudioManager::Init() {
	engine = new FMODEngine;
}

// Updates the manager and FMOD engine. Should run every frame.
void AudioManager::Update() {
	engine->Update();
}

// Loads a sound into FMOD's memory and into the internal sounds map.
void AudioManager::Load(std::string name, bool loop = false) {
	if (engine->sounds.find(name) == engine->sounds.end()) {
		FMOD_MODE fmod_mode = 
			FMOD_2D | 
			FMOD_CREATECOMPRESSEDSAMPLE | 
			(loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
		FMOD::Sound* sound = nullptr;
		engine->system->createSound(name.c_str(), fmod_mode, nullptr, &sound);
		engine->sounds[name] = sound;
	}
}

// Loads a sound and tells FMOD to loop it when playing.
void AudioManager::LoadLoop(std::string name) {
	Play(name, true);
}

// Unloads a sound from FMOD's memory.
void AudioManager::Unload(std::string name) {
	auto sound = engine->sounds.find(name);
	if (sound != engine->sounds.end()) {
		sound->second->release();
		engine->sounds.erase(sound);
	}
}

// Plays a sound. Will load the sound using the "Load" function if it
// has not already been loaded.
// 
// The volume value given is divided by 100 when being sent to FMOD,
// as FMOD's volume operates between 0 and 1.
int AudioManager::Play(std::string name, float x = 0, float y = 0, float volume = 1.f) {
	auto sound = engine->sounds.find(name);
	if (sound == engine->sounds.end()) {
		std::cout << name << " was not preloaded, loading from source instead.";
		AudioManager::Load(name);
		sound = engine->sounds.find(name);
	}

	if (sound != engine->sounds.end()) {
		FMOD::Channel* channel = nullptr;
		engine->system->playSound(sound->second, nullptr, true, &channel);
		FMOD_VECTOR position = { x, y, 0 };
		channel->set3DAttributes(&position, nullptr);
		channel->setVolume(volume / VOLUME_DIV);
		channel->setPaused(false);
		engine->channels[engine->newChannelID] = channel;
		return engine->newChannelID++;
	}
	else {
		std::cout << name << " could not be loaded.";
		return -1;
	}
}

int AudioManager::Play(std::string name, Vector2D position, float volume = 1.f) {
	AudioManager::Play(name, position.x, position.y, volume);
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
		channel->second->setPaused(paused);
}

// Sets volume on a channel between 1 and 100.
// The volume value given is divided by 100 when being sent to FMOD,
// as FMOD's volume operates between 0 and 1.
void AudioManager::SetVolume(int channelID, float volume) {
	auto channel = engine->channels.find(channelID);
	if (channel != engine->channels.end())
		channel->second->setVolume(volume / VOLUME_DIV);
}

// Sets the channel's world position.
void AudioManager::SetPosition(int channelID, float x, float y) {
	auto channel = engine->channels.find(channelID);
	if (channel != engine->channels.end()) {
		FMOD_VECTOR position = {x, y, 0};
		channel->second->set3DAttributes(&position, NULL);
	}
}

// Sets the channel's world position.
void AudioManager::SetPosition(int channelID, Vector2D position) {
	AudioManager::SetPosition(channelID, position.x, position.y);
}

void AudioManager::LoadBank(std::string name) {
	if (engine->banks.find(name) == engine->banks.end()) {
		FMOD::Studio::Bank* bank;
		engine->studioSystem->loadBankFile(name.c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &bank);
		if (bank) {
			engine->banks[name] = bank;
		}
	}
}

void AudioManager::LoadEvent(std::string name) {
	auto eventIterator = engine->events.find(name);
	if (eventIterator == engine->events.end()) {
		FMOD::Studio::EventDescription* eventDescription = nullptr;
		FMOD::Studio::EventInstance* eventInstance = nullptr;
		engine->studioSystem->getEvent(name.c_str(), &eventDescription);

		if (eventDescription) {
			eventDescription->createInstance(&eventInstance);
			if (eventInstance)
				engine->events[name] = eventInstance;
			else
				std::cout << name << " event instance could not be loaded.";
		}
		else 
			std::cout << name << " event description could not be loaded.";
	}
}