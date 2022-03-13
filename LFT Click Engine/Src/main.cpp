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

std::unique_ptr<DebugRenderer> g_DebugRenderer;
std::unique_ptr<Graphics> g_Renderer;

int main(int argc, char* args[])
{
	int windowWidth = 800, windowHeight = 600;
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
			windowWidth = (float)lua_tonumber(L, -1);
		}

		lua_getglobal(L, "windowHeight");
		if (lua_isnumber(L, -1))
		{
			windowHeight = (float)lua_tonumber(L, -1);
		}
	}
#endif

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
	{
		std::cout << "SDL_Init failled, erorr" << std::endl;
		return 1;
	}

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
	SDL_Surface* icon = ResourceManager::getInstance().GetResource("Resources\\images\\icon.bmp");
	SDL_SetWindowIcon(pWindow, icon);


	g_Renderer = std::make_unique<Graphics>();
	g_Renderer->Initialize(GetActiveWindow(), windowWidth, windowHeight);

	g_DebugRenderer = std::make_unique<DebugRenderer>(g_Renderer->GetDevice(), g_Renderer->GetContext());

	////imgui setup
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplSDL2_InitForD3D(pWindow);
	ImGui_ImplDX11_Init(g_Renderer->GetDevice(), g_Renderer->GetContext());


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

		std::fstream other("./Resources/json/demo.json");
		//std::fstream other("./Resources/json/concept_3_level.json");
		json dataJson2;
		other >> dataJson2;
		other.close();

		gom->Deserialize(gof, dataJson2);

		GameObject* playerObj = gom->FindObjectOfTag("player");
		GameManager::getInstance().mainCamera = playerObj->getComponent<Camera>();

		//gom->Start();
		isRunning = true;
		unsigned int lastTime = 0;
		FrameRateController::getInstance().Init(144);

		while (isRunning)
		{
			FrameRateController::getInstance().Tick();

			ImGui_ImplDX11_NewFrame();
			ImGui_ImplSDL2_NewFrame();
			ImGui::NewFrame();

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
				if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					windowWidth = e.window.data1;
					windowHeight = e.window.data2;
					SDL_SetWindowSize(pWindow, windowWidth, windowHeight);
					g_Renderer->OnResize(windowWidth, windowHeight);
				}
			}
			AudioManager::getInstance().Update();
			InputManager::getInstance().Update();
			gom->Update();
			EventManager::getInstance().ProcessCollision();
			//gom->DoCollision(playerObj);//handle colision with respect to player, this will need to change
			EventManager::getInstance().Update();

			g_Renderer->PrepareForRendering();

			gom->Draw();

			g_DebugRenderer->Draw(g_Renderer->GetContext(), g_Renderer->GetWidth(), g_Renderer->GetHeight());


			bool open = true;

			ImGui::SetNextWindowPos({ 0,0 });
			ImGui::Begin("2ndWindow", &open, ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoBackground);
			ImGui::Text("FPS: %03f", 1.0f / FrameRateController::getInstance().DeltaTime());
			ImGui::End();
			if (GameManager::getInstance().playerDead)
			{
				ImGui::SetNextWindowPos({ (float)((windowWidth / 2) - 50),(float)((windowHeight / 2) + 100) });
				ImGui::Begin("mainMenu", &open, ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize);
				if (ImGui::Button("Restart", { 100,50 }) || GameManager::getInstance().playerRestart)
				{
					isRunning = false;
					playGame = true;
					doMenu = false;
					masterLoop = true;
				}
				//if (ImGui::Button("Main Menu", { 100,50 }))
				//{
				//	isRunning = false;
				//	playGame = false;
				//	doMenu = true;
				//	masterLoop = true;
				//}
				if (ImGui::Button("Quit", { 100,50 }))
				{
					isRunning = false;
					playGame = false;
					doMenu = false;
					masterLoop = false;
				}

				ImGui::End();
			}

			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
			g_Renderer->PresentFrame();

		}

		gom->DeleteAll();

		EventManager::getInstance().Reset();
		GameManager::getInstance().playerDead = false;
		GameManager::getInstance().playerRestart = false;
		GameManager::getInstance().playerScore = 0;

	}
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	g_DebugRenderer.reset();

	AudioManager::getInstance().Term();
	SDL_DestroyWindow(pWindow);
#ifdef _DEBUG
	lua_close(L);
#endif
	SDL_Quit();

	return 0;
}
