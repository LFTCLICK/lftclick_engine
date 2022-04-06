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
#include <WICTextureLoader.h>
#include "..\LuaManager.h"

using json = nlohmann::json;

using namespace DirectX;

constexpr auto INITIAL_WINDOW_WIDTH = 800;
constexpr auto INITIAL_WINDOW_HEIGHT = 600;

std::unique_ptr<DebugRenderer> g_DebugRenderer;
std::unique_ptr<Renderer> g_Renderer;
std::unique_ptr<FrameRateController> g_FrameRateController;
std::unique_ptr<InputManager> g_InputManager;
std::unique_ptr<GameObjectManager> g_GameObjManager;
std::unique_ptr<GameObjectFactory> g_GameObjFactory;
std::unique_ptr<EventManager> g_EventManager;
std::unique_ptr<GameManager> g_GameManager;
std::unique_ptr<AudioManager> g_AudioManager;
std::unique_ptr<LuaManager> g_LuaManager;
std::unique_ptr<AStarTerrain> g_AStarTerrain;

int main(int argc, char* args[])
{
	HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"CoInitializeEx failed", 0, MB_OK | MB_ICONERROR);
		return -1;
	}

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
	{
		std::cout << "SDL_Init failed, erorr" << std::endl;
		return 1;

	}

	SDL_Window* pWindow = SDL_CreateWindow("LFT Click Engine Demo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	if (!pWindow)
		return 1;

	SDL_Surface* icon = ResourceManager::getInstance().GetResource("Resources\\images\\icon.bmp");
	SDL_SetWindowIcon(pWindow, icon);

	g_LuaManager = std::make_unique<LuaManager>();
	g_FrameRateController = std::make_unique<FrameRateController>();
	g_InputManager = std::make_unique<InputManager>();
	g_GameObjManager = std::make_unique<GameObjectManager>();
	g_GameObjFactory = std::make_unique<GameObjectFactory>();
	g_EventManager = std::make_unique<EventManager>();

	g_GameManager = std::make_unique<GameManager>();

	g_AudioManager = std::make_unique<AudioManager>();
	g_AStarTerrain = std::make_unique<AStarTerrain>();

	g_Renderer = std::make_unique<Renderer>();
	g_Renderer->Initialize(GetActiveWindow(), INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT);
	g_Renderer->InitImGui(pWindow);

	g_GameManager->windowHeight = g_Renderer->GetWidth();
	g_GameManager->windowWidth = g_Renderer->GetHeight();

	g_EventManager->init(g_GameObjManager.get());
	g_DebugRenderer = std::make_unique<DebugRenderer>(g_Renderer->GetDevice(), g_Renderer->GetContext());

	g_FrameRateController->Init(144);
	g_AudioManager->Init();

	std::fstream other("./Resources/json/survival.json");

	json dataJson2;
	other >> dataJson2;
	other.close();

	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFileEx(g_Renderer->GetDevice(), 
			L"Resources\\images\\mainMenu_background.png", 0, 
			D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0, DirectX::WIC_LOADER_IGNORE_SRGB, nullptr, 
			g_GameManager->menuBackgroundSRV.ReleaseAndGetAddressOf()) );

	srand(time(NULL));

	SDL_Event e = {};

	while (e.type != SDL_QUIT)
	{
		SDL_PollEvent(&e);

		switch (e.type)
		{
		case SDL_WINDOWEVENT:
			if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) 
			{
				SDL_SetWindowSize(pWindow, e.window.data1, e.window.data2);
				g_Renderer->OnResize(e.window.data1, e.window.data2);
			}
			break;
		}

		g_FrameRateController->Tick();
		g_Renderer->PrepareForRendering();

		switch (g_GameManager->currentLevel)
		{
		case EGameLevel::Mainmenu:

			RECT rect;
			rect.left = rect.top = 0;
			rect.right = g_Renderer->GetWidth();
			rect.bottom = g_Renderer->GetHeight();
			g_Renderer->GetSpriteBatch()->Draw(g_GameManager->menuBackgroundSRV.Get(), rect);

			ImGui::SetNextWindowPos(ImVec2(static_cast<float>(g_Renderer->GetWidth())/2 - 50, static_cast<float>(g_Renderer->GetHeight())/2));
			ImGui::Begin("menu", nullptr, 
				ImGuiWindowFlags_::ImGuiWindowFlags_NoMove|ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar 
				|ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize| ImGuiWindowFlags_NoBackground);

			if (ImGui::Button("Play", { 100,50 }))
			{
				g_GameManager->LoadLevel(dataJson2);
			}
			
			if (ImGui::Button("Quit", { 100, 50 }))
			{
				e.type = SDL_QUIT;
			}

			ImGui::End();

			break;


		case EGameLevel::Level0:
		case EGameLevel::Pausemenu:

			if (g_InputManager->isKeyTriggered(SDL_SCANCODE_ESCAPE))
			{
				if (!(g_GameManager->currentLevel == EGameLevel::Pausemenu))
					g_GameManager->currentLevel = EGameLevel::Pausemenu;
				else
				{
					g_GameManager->currentLevel = EGameLevel::Level0;
					g_FrameRateController->zeroDeltaTime = false;
				}
			}

			if (g_GameManager->currentLevel == EGameLevel::Pausemenu)
			{
				g_FrameRateController->zeroDeltaTime = true;
				ImGui::SetNextWindowPos(ImVec2(static_cast<float>(g_Renderer->GetWidth()) / 2 - 50, static_cast<float>(g_Renderer->GetHeight()) / 2));
				ImGui::Begin("pauseMenu", nullptr,
					ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar
					| ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);

				if (g_GameManager->playerDead)
				{
					if (ImGui::Button("Restart", { 100,50 }))
					{
						g_GameManager->LoadLevel(dataJson2);
					}
				}
				else
				{
					if (ImGui::Button("Continue", { 100, 50 }))
					{
						g_GameManager->currentLevel = EGameLevel::Level0;
						g_FrameRateController->zeroDeltaTime = false;
					}
				}

				if (ImGui::Button("Main Menu", { 100,50 }))
				{
					g_GameManager->currentLevel = EGameLevel::Mainmenu;
				}

				if (ImGui::Button("Quit", { 100, 50 }))
				{
					e.type = SDL_QUIT;
				}

				ImGui::End();
			}

			g_AudioManager->Update();
			g_GameManager->Update();
			g_InputManager->Update();
			g_GameObjManager->Update();
			g_EventManager->Update();
			g_LuaManager->Update();

			g_GameObjManager->Draw();
			g_Renderer->Draw();

#ifdef _DEBUG
			g_DebugRenderer->Draw(g_Renderer->GetContext(), g_Renderer->GetWidth(), g_Renderer->GetHeight());
#endif
			break;
		default:
			assert("fix ur shit");
			break;
		}

		g_Renderer->PresentFrame();
	}

	CoUninitialize();

	g_GameObjManager->DeleteAll();

	g_EventManager->Reset();
	g_GameManager->playerDead = false;

	SDL_DestroyWindow(pWindow);
	SDL_Quit();

	return 0;
}

