#pragma once
// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	LuaManager.h
// Author			:	Rohit Punjabi
// Creation Date	:	03/13/2022
// Purpose			:	Takes care of binding LUA functions
// History			:
// ---------------------------------------------------------------------------
#include <sol/sol.hpp>
#include "Components/GameObject.h"
#include <string>

// Luabinding wrapper class using SOL
class LuaManager
{
	sol::state lua_manager_state;

public:

	static LuaManager& getInstance()
	{
		static LuaManager instance;
		return instance;
	}

	LuaManager();
	~LuaManager();
	void Update();
	void RegObjectFunctions(sol::state& state, GameObject* obj);
	void RegGlobals(sol::state& state);
	void print();

	/*Call SDL_Log
	* accessed from inside Lua script
	* Returns: void
	*/
	//void LogMessage(std::string log_str);
};
