#pragma once
// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	LuaManager.h
// Author			:	Rohit Punjabi
// Creation Date	:	03/13/2022
// Purpose			:	Takes care of binding LUA functions
// History			:
// ---------------------------------------------------------------------------
#include "InputManager.h"
#include "GameObjectManager.h"
#include "Components/GameObject.h"
#include "Components/Player.h"
#include "Components/Camera.h"
#include "Components/Gun.h"
#include "Components/Transform.h"
#include "FrameRateController.h"
#include <sol/sol.hpp>

// Luabinding wrapper class using SOL
class LuaManager
{
	sol::state lua_manager_state;

public:

	LuaManager();
	~LuaManager();
	void Update();
	void RegObjectFunctions(sol::state& state, GameObject* obj);
	void RegGlobals(sol::state& state);

	/*Call SDL_Log
	* accessed from inside Lua script
	* Returns: void
	*/
	//void LogMessage(std::string log_str);
};

extern std::unique_ptr<LuaManager> g_LuaManager;