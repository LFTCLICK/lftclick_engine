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
#include "GameObjectFactory.h"
#include "GameObjectManager.h"
#include "Graphics.h"
#include "Components/Transform.h"
#include "EventManager.h"
#include "AudioManager.h"
#include "Messages.h"
#include "GameManager.h"
#include "Components/Drawable.h"
using json = nlohmann::json;

using namespace DirectX;

std::unique_ptr<DebugRenderer> g_debugRenderer;

int main(int argc, char* args[])
{
	int windowWidth = 800, windowHeight = 600;

	// Create Lua State
	lua_State* L = luaL_newstate();

	// Adding standard libraries to Lua Virtual Machine
	luaL_openlibs(L);

	if (CheckLua(L, luaL_dofile(L, "Resources\\LuaScripts\\ConfigurationScript.lua")))
	{
		lua_getglobal(L, "windowWidth");
		if (lua_isnumber(L, -1))
		{
			windowWidth = (float)lua_tonumber(L, -1);
		}

		lua_getglobal(L, "windowHeight");
		if (lua_isnumber(L, -1))
		{
			windowHeight = (float)lua_tonumber(L, -1);
		}
	}

	//Init SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
	{
		std::cout << "SDL_Init failled, erorr" << std::endl;
		return 1;
	}

	//Create SDL window
	SDL_Window* pWindow = SDL_CreateWindow("LFT Click Engine Demo",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		windowWidth,
		windowHeight,
		SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	if (pWindow == NULL)
	{
		std::cout << "Couldn't create window " << SDL_GetError();
		return 1;
	}
	//load in and set icon
	SDL_Surface* icon = ResourceManager::getInstance().GetResource("Resources\\images\\icon.bmp");
	SDL_SetWindowIcon(pWindow, icon);

	Graphics::getInstance().Initialize(GetActiveWindow(), windowWidth, windowHeight);

	g_debugRenderer = std::make_unique<DebugRenderer>(&Graphics::getInstance());

	////imgui setup
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplSDL2_InitForD3D(pWindow);
	ImGui_ImplDX11_Init(Graphics::getInstance().GetDevice(), Graphics::getInstance().GetContext());

	std::fstream data("Resources/json/menu.json");
	json dataJson;
	data >> dataJson;
	data.close();

	GameObjectManager* gom = &GameObjectManager::getInstance();
	EventManager::getInstance().init(gom);
	GameObjectFactory* gof = &GameObjectFactory::getInstance();


	FrameRateController::getInstance().Init(144);
	AudioManager::getInstance().Init();
	bool masterLoop = true;
	bool playGame = false;
	bool doMenu = true;
	srand(time(NULL));

	bool isRunning = true;
	while (masterLoop)
	{
		//if (playGame)
		if (true)
		{
			std::fstream other("./Resources/json/demo.json");
			json dataJson2;
			other >> dataJson2;
			other.close();
			gom->Deserialize(gof, dataJson2);
			GameObject* playerObj = gom->FindObjectOfTag("player");
			GameManager::getInstance().mainCamera = playerObj->getComponent<Camera>();

			//gom->Start();
			isRunning = true;
			unsigned int lastTime = 0;
			FrameRateController::getInstance().Init(144);//if there has been a considerable gap between EndOfFrame and StartOfFrame call this first so that the first delta time isn't absurdly long
			while (isRunning)
			{
				FrameRateController::getInstance().Tick();

				ImGui_ImplDX11_NewFrame();
				ImGui_ImplSDL2_NewFrame();
				ImGui::NewFrame();

				SDL_Event e;
				while (SDL_PollEvent(&e) != 0)//must be called before input manager
				{
					if (e.type == SDL_QUIT)
					{
						isRunning = false;
						playGame = false;
						doMenu = false;
						masterLoop = false;
					}
					if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_RESIZED)
					{
						windowWidth = e.window.data1;
						windowHeight = e.window.data2;
						SDL_SetWindowSize(pWindow, windowWidth, windowHeight);
						Graphics::getInstance().OnResize(windowWidth, windowHeight);
					}
				}
				AudioManager::getInstance().Update();
				InputManager::getInstance().Update();
				gom->Update();//update gameobjects
				EventManager::getInstance().ProcessCollision();
				//gom->DoCollision(playerObj);//handle colision with respect to player, this will need to change
				EventManager::getInstance().Update();//process timed events
				Graphics::getInstance().ClearBuffer(0x7CA3FF);

				g_debugRenderer->DrawLine(SimpleMath::Vector2(0.0f, 0.0f), SimpleMath::Vector2(Graphics::getInstance().GetWidth() / 2, Graphics::getInstance().GetHeight() / 2));
				g_debugRenderer->DrawLine(SimpleMath::Vector2(Graphics::getInstance().GetWidth(), 0.0f), SimpleMath::Vector2(Graphics::getInstance().GetWidth() / 2, Graphics::getInstance().GetHeight() / 2));

				g_debugRenderer->DrawQuad(SimpleMath::Vector2(100.0f, 50.0f), SimpleMath::Vector2(150.0f, 50.0f),
					SimpleMath::Vector2(150.0f, 100.0f), SimpleMath::Vector2(100.0f, 100.0f));


				gom->Draw();

				g_debugRenderer->Draw(&Graphics::getInstance());


				bool open = true;

				ImGui::SetNextWindowPos({ 0,0 });
				ImGui::Begin("2ndWindow", &open, ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoBackground);
				ImGui::Text("FPS: %03f", 1.0f / FrameRateController::getInstance().DeltaTime());
				ImGui::End();


				ImGui::Render();
				ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
				Graphics::getInstance().EndFrame();

			}

			gom->DeleteAll();

			EventManager::getInstance().Reset();
			GameManager::getInstance().playerDead = false;
			GameManager::getInstance().playerRestart = false;
			GameManager::getInstance().playerScore = 0;

		}

		// Cleanup
		//gom->DeleteAll();
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();

		g_debugRenderer.reset();

		AudioManager::getInstance().Term();
		SDL_DestroyWindow(pWindow);

		lua_close(L);
		SDL_Quit();

		return 0;
	}
}
