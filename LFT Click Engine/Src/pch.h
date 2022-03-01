#pragma once

#include <iostream>
#include <SDL.h>
#include <SDL_syswm.h>
#include <Windows.h>
#include <fstream>
#include <json.hpp>
#include <time.h>
#include <memory.h>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>
#include <SDL_joystick.h>
#include <SDL_gamecontroller.h>
#include <directxmath.h>
#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXColors.h>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_dx11.h"

#include "SimpleMath.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

// Integrating LUA
extern "C"
{
#include "LUA/include/lauxlib.h"
#include "LUA/include/lua.h"
#include "LUA/include/lualib.h"
#include "LUA/include/luaconf.h"
}

// Linking lua library
#ifdef _WIN64
#pragma comment(lib, "liblua54.a")
#endif

namespace DX
{
	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			throw std::exception();
		}
	}
}

// Error checking for LUA
bool CheckLua(lua_State* L, int r);


inline DirectX::SimpleMath::Vector2 ToVanceCoords(DirectX::SimpleMath::Vector2 mycoords, float w, float h)
{
	return DirectX::SimpleMath::Vector2(mycoords.x + w / 2.0f, -(mycoords.y - h / 2.0f));
}