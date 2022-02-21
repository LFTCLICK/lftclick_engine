// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	ResourceManager.cpp
// Author			:	Vance Howald
// Creation Date	:	2021/10/06
// Purpose			:	Implementation
// History			:
// ---------------------------------------------------------------------------
#include "pch.h"
#include "ResourceManager.h"

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
	for (auto element : resourceMap)
	{
		SDL_FreeSurface(element.second);
	}
	resourceMap.clear();
}

SDL_Surface * ResourceManager::GetResource(std::string id)
{
	auto pos = resourceMap.find(id);
	if (pos != resourceMap.end())
		return pos->second;
	
	std::pair<std::string, SDL_Surface*> toAdd(id, SDL_LoadBMP(id.c_str()));
	if (toAdd.second == nullptr)
	{
		free(&toAdd);
		return nullptr;
	}
	resourceMap.insert(toAdd);
	return toAdd.second;
}
