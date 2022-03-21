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