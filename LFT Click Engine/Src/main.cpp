// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	main.cpp
// Author			:	Vance Howald
// Creation Date	:	2021/10/06
// Purpose			:	implementation of the 'play' game state
// History			:
// 2021/10/29		-	Added component based arch
// 2021/12/01		-	Added messaging
// ---------------------------------------------------------------------------
#define no_init_all deprecated

#include "pch.h"
#include "InputManager.h"
#include "FrameRateController.h"
#include "ResourceManager.h"
#include "AStarTerrain.h"
#include "GameObjectFactory.h"
#include "GameObjectManager.h"
#include "Renderer.h"
#include "Components/Transform.h"
#include "EventManager.h"
#include "AudioManager.h"
#include "Messages.h"
#include "GameManager.h"
#include "Components/Drawable.h"
#include "DebugRenderer.h"

using json = nlohmann::json;

using namespace DirectX;

int g_WindowWidth = 800;
int g_WindowHeight = 600;

std::unique_ptr<DebugRenderer> g_DebugRenderer;
std::unique_ptr<Renderer> g_Renderer;
std::unique_ptr<FrameRateController> g_FrameRateController;
std::unique_ptr<InputManager> g_InputManager;
std::unique_ptr<GameObjectManager> g_GameObjManager;
std::unique_ptr<GameObjectFactory> g_GameObjFactory;
std::unique_ptr<EventManager> g_EventManager;
std::unique_ptr<GameManager> g_GameManager;
std::unique_ptr<AudioManager> g_AudioManager;
std::unique_ptr<AStarTerrain> g_AStarTerrain;

int main(int argc, char* args[])
{
#ifdef _DEBUG
	// Create Lua State
	lua_State* L = luaL_newstate();

	// Adding standard libraries to Lua Virtual Machine
	luaL_openlibs(L);

	if (CheckLua(L, luaL_dofile(L, "Resources\\LuaScripts\\ConfigurationScript.lua")))
	{
		lua_getglobal(L, "windowWidth");
		if (lua_isnumber(L, -1))
		{
			g_WindowWidth = (float)lua_tonumber(L, -1);
		}

		lua_getglobal(L, "windowHeight");
		if (lua_isnumber(L, -1))
		{
			g_WindowHeight = (float)lua_tonumber(L, -1);
		}
	}
#endif

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
		return 1;

	SDL_Window* pWindow = SDL_CreateWindow("LFT Click Engine Demo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, g_WindowWidth, g_WindowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	if (!pWindow)
		return 1;

	SDL_Surface* icon = ResourceManager::getInstance().GetResource("Resources\\images\\icon.bmp");
	SDL_SetWindowIcon(pWindow, icon);

	g_FrameRateController = std::make_unique<FrameRateController>();
	g_InputManager = std::make_unique<InputManager>();
	g_GameObjManager = std::make_unique<GameObjectManager>();
	g_GameObjFactory = std::make_unique<GameObjectFactory>();
	g_EventManager = std::make_unique<EventManager>();
	g_GameManager = std::make_unique<GameManager>();
	g_AudioManager = std::make_unique<AudioManager>();
	g_AStarTerrain = std::make_unique<AStarTerrain>();

	g_Renderer = std::make_unique<Renderer>();
	g_Renderer->Initialize(GetActiveWindow(), g_WindowWidth, g_WindowHeight);
	g_Renderer->InitImGui(pWindow);

	g_DebugRenderer = std::make_unique<DebugRenderer>(g_Renderer->GetDevice(), g_Renderer->GetContext());

	g_EventManager->init(g_GameObjManager.get());

	g_FrameRateController->Init(144);

	g_AudioManager->Init();

	bool masterLoop = true;
	bool playGame = false;
	bool doMenu = true;
	srand(time(NULL));

	bool isRunning = true;
	while (masterLoop)
	{
		std::fstream other("./Resources/json/survival.json");
		//std::fstream other("./Resources/json/demo.json");
		//std::fstream other("./Resources/json/concept_3_level.json");

		json dataJson2;
		other >> dataJson2;
		other.close();

		g_GameObjManager->Deserialize(g_GameObjFactory.get(), dataJson2);

		GameObject* playerObj = g_GameObjManager->FindObjectOfTag("player");
		g_GameManager->playerObj = playerObj;
		g_GameManager->mainCamera = playerObj->getComponent<Camera>();
		g_GameManager->playerTrans = playerObj->getComponent<Transform>();

		isRunning = true;
		g_FrameRateController->Init(144);

		while (isRunning)
		{
			g_FrameRateController->Tick();
			g_Renderer->PrepareForRendering();

			SDL_Event e;
			while (SDL_PollEvent(&e) != 0)
			{
				if (e.type == SDL_QUIT)
				{
					isRunning = false;
					playGame = false;
					doMenu = false;
					masterLoop = false;
				}
				/*if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					g_WINDOW_WIDTH = e.window.data1;
					g_WINDOW_HEIGHT = e.window.data2;
					SDL_SetWindowSize(pWindow, g_WINDOW_WIDTH, g_WINDOW_HEIGHT);
					g_Renderer->OnResize(g_WINDOW_WIDTH, g_WINDOW_HEIGHT);
				}*/
			}
			g_AudioManager->Update();
			g_GameManager->UpdateTime();
			g_InputManager->Update();
			g_GameObjManager->Update();
			g_EventManager->ProcessCollision();
			g_EventManager->Update();


			g_GameObjManager->Draw();
			g_Renderer->Draw();

#ifdef _DEBUG
			g_DebugRenderer->Draw(g_Renderer->GetContext(), g_Renderer->GetWidth(), g_Renderer->GetHeight());
#endif
			bool open = true;


			if (g_GameManager->playerDead)
			{
				ImGui::SetNextWindowPos({ (float)((g_WindowWidth / 2) - 50),(float)((g_WindowHeight / 2) + 100) });
				ImGui::Begin("mainMenu", &open, ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize);
				if (ImGui::Button("Restart", { 100,50 }) || g_GameManager->playerRestart)
				{
					isRunning = false;
					playGame = true;
					doMenu = false;
					masterLoop = true;
				}

				if (ImGui::Button("Quit", { 100,50 }))
				{
					isRunning = false;
					playGame = false;
					doMenu = false;
					masterLoop = false;
				}

				ImGui::End();
			}

			g_Renderer->PresentFrame();
		}

		g_GameObjManager->DeleteAll();

		g_EventManager->Reset();
		g_GameManager->playerDead = false;
		g_GameManager->playerRestart = false;
		g_GameManager->playerScore = 0;

	}


	g_DebugRenderer.reset();
	g_FrameRateController.reset();
	g_Renderer.reset();

	g_AudioManager->Term();

	SDL_DestroyWindow(pWindow);
#ifdef _DEBUG
	lua_close(L);
#endif
	SDL_Quit();

	return 0;
}
