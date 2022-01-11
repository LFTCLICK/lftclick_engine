// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	ResourceManager.h
// Author			:	Vance Howald
// Creation Date	:	2021/10/06
// Purpose			:	Loads resources on call and caches pointers for multiple instances of the same texture
// History			: 
// - 2021/10/29		Singleton
// ---------------------------------------------------------------------------
#pragma once
#include <string>
#include <SDL.h>
#include <unordered_map>

class ResourceManager{
public:
	static ResourceManager& getInstance()
	{
		static ResourceManager instance;
		return instance;
	}
	ResourceManager(ResourceManager const&) = delete;
    void operator=(ResourceManager const&) = delete;
	~ResourceManager();
	SDL_Surface* GetResource(std::string id);
	//?????* GetResource(std::string id); //Get resource for directx

private:
	ResourceManager();
	int maxSize;
	std::unordered_map<std::string, SDL_Surface*> resourceMap;
};