// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	main.cpp
// Author			:	Vance Howald
// Creation Date	:	2021/10/06
// Purpose			:	main game loop
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

constexpr auto INITIAL_WINDOW_WIDTH = 1600;
constexpr auto INITIAL_WINDOW_HEIGHT = 900;

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

SDL_Window* g_pWindow;

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
		MessageBox(NULL, L"SDL_Init failed, error", 0, MB_OK | MB_ICONERROR);
		return 1;

	}

	UINT windowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;

#ifndef _DEBUG
	windowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
#endif

	g_pWindow = SDL_CreateWindow("LFT Click Engine Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, windowFlags);

	if (!g_pWindow)
		return 1;

	SDL_Surface* icon = ResourceManager::getInstance().GetResource("Resources\\images\\icon.bmp");
	SDL_SetWindowIcon(g_pWindow, icon);

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
	g_Renderer->InitImGui(g_pWindow);

	g_GameManager->windowHeight = g_Renderer->GetWidth();
	g_GameManager->windowWidth = g_Renderer->GetHeight();

	g_EventManager->init(g_GameObjManager.get());
	g_DebugRenderer = std::make_unique<DebugRenderer>(g_Renderer->GetDevice(), g_Renderer->GetContext());

	g_FrameRateController->Init(144);
	g_AudioManager->Init();

	SDL_Event e = {};

	std::ifstream intro("./Resources/json/intro.json");

	json introJson2;
	intro >> introJson2;
	intro.close();
	g_GameManager->LoadLevel(introJson2, EGameLevel::Intro);
	int currentImage = 0;
	float timer = 0;
	g_GameObjManager->FindObjectOfTag("FMOD_logo")->isOnScreen = false;
	g_GameObjManager->FindObjectOfTag("digi_logo")->isOnScreen = true;
	while (e.type != SDL_QUIT)
	{
		SDL_PollEvent(&e);

		switch (e.type)
		{
		case SDL_WINDOWEVENT:
			if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
			{
				SDL_SetWindowSize(g_pWindow, e.window.data1, e.window.data2);
				g_Renderer->OnResize(e.window.data1, e.window.data2);
			}
			break;
		}

		g_FrameRateController->Tick();
		g_Renderer->PrepareForRendering();

		timer += g_FrameRateController->DeltaTime();
		if (currentImage == 0 && timer >= 2.0f)
		{
			currentImage++;
			g_GameObjManager->FindObjectOfTag("FMOD_logo")->isOnScreen = true;
			g_GameObjManager->FindObjectOfTag("digi_logo")->isOnScreen = false;
			//switch
		}
		if (currentImage == 1 && timer >= 4.0f)
		{
			g_InputManager->Update();
			//g_GameObjManager->Update();
			//g_EventManager->Update();
			//g_LuaManager->Update();

			g_GameObjManager->Draw();
			g_Renderer->Draw(DirectX::Colors::Black);

			g_Renderer->PresentFrame();
			break;
		}

		//g_AudioManager->Update();
		//g_GameManager->Update();
		g_InputManager->Update();
		//g_GameObjManager->Update();
		//g_EventManager->Update();
		//g_LuaManager->Update();

		g_GameObjManager->Draw();
		g_Renderer->Draw(DirectX::Colors::Black);

		g_Renderer->PresentFrame();
	}

	g_GameObjManager->DeleteAll();

	std::ifstream other("./Resources/json/survival.json");

	json dataJson2;
	other >> dataJson2;
	other.close();
	//g_GameManager->LoadLevel(dataJson2, EGameLevel::Intro);

	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFileEx(g_Renderer->GetDevice(),
			L"Resources\\images\\mainMenu_background.png", 0,
			D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0, DirectX::WIC_LOADER_IGNORE_SRGB, nullptr,
			g_GameManager->menuBackgroundSRV.ReleaseAndGetAddressOf()));
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFileEx(g_Renderer->GetDevice(),
			L"Resources\\images\\credits.png", 0,
			D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0, DirectX::WIC_LOADER_IGNORE_SRGB, nullptr,
			g_GameManager->creditsSRV.ReleaseAndGetAddressOf()));
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFileEx(g_Renderer->GetDevice(),
			L"Resources\\images\\controls.png", 0,
			D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0, DirectX::WIC_LOADER_IGNORE_SRGB, nullptr,
			g_GameManager->controlsSRV.ReleaseAndGetAddressOf()));
	
	srand(time(NULL));
	g_GameManager->currentLevel = EGameLevel::Mainmenu;
	RECT rect;
	while (e.type != SDL_QUIT)
	{
		SDL_PollEvent(&e);

		switch (e.type)
		{
		case SDL_WINDOWEVENT:
			if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
			{
				SDL_SetWindowSize(g_pWindow, e.window.data1, e.window.data2);
				g_Renderer->OnResize(e.window.data1, e.window.data2);
			}
			break;
		}

		g_FrameRateController->Tick();
		g_Renderer->PrepareForRendering();

		switch (g_GameManager->currentLevel)
		{

		case EGameLevel::Mainmenu:
			rect.left = rect.top = 0;
			rect.right = g_Renderer->GetWidth();
			rect.bottom = g_Renderer->GetHeight();
			g_Renderer->GetSpriteBatch()->Draw(g_GameManager->menuBackgroundSRV.Get(), rect);

			ImGui::SetNextWindowPos(ImVec2(static_cast<float>(g_Renderer->GetWidth()) / 2 - 50, static_cast<float>(g_Renderer->GetHeight()) / 2));
			ImGui::Begin("menu", nullptr,
				ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar
				| ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);

			if (ImGui::Button("Play", { 100,50 }))
			{
				g_GameManager->LoadLevel(dataJson2, EGameLevel::Level0);
			}
			if (ImGui::Button("Credits", { 100,50 }))
			{
				g_GameManager->prevLevel = g_GameManager->currentLevel;
				g_GameManager->currentLevel = EGameLevel::CreditsScreen;
			}if (ImGui::Button("Controls", { 100,50 }))
			{
				g_GameManager->prevLevel = g_GameManager->currentLevel;
				g_GameManager->currentLevel = EGameLevel::ControlScreen;
			}

			if (ImGui::Button("Quit", { 100, 50 }))
			{
				e.type = SDL_QUIT;
			}

			ImGui::End();
			break;
		case EGameLevel::CreditsScreen:
			rect.left = rect.top = 0;
			rect.right = g_Renderer->GetWidth();
			rect.bottom = g_Renderer->GetHeight();
			g_Renderer->GetSpriteBatch()->Draw(g_GameManager->creditsSRV.Get(), rect);

			ImGui::SetNextWindowPos(ImVec2(static_cast<float>(g_Renderer->GetWidth()) / 2 - 50, static_cast<float>(3 * g_Renderer->GetHeight()) / 4));
			ImGui::Begin("menu", nullptr,
				ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar
				| ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);

			if (ImGui::Button("Back", { 100,50 }))
			{
				g_GameManager->currentLevel = g_GameManager->prevLevel;
			}

			ImGui::End();

			g_Renderer->Draw(DirectX::Colors::Black);
			break;

		case EGameLevel::ControlScreen:

			rect.left = rect.top = 0;
			rect.right = g_Renderer->GetWidth();
			rect.bottom = g_Renderer->GetHeight();
			g_Renderer->GetSpriteBatch()->Draw(g_GameManager->controlsSRV.Get(), rect);

			ImGui::SetNextWindowPos(ImVec2(static_cast<float>(g_Renderer->GetWidth()) / 2 - 50, static_cast<float>(3 * g_Renderer->GetHeight()) / 4));
			ImGui::Begin("menu", nullptr,
				ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar
				| ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);

			if (ImGui::Button("Back", { 100,50 }))
			{
				g_GameManager->currentLevel = g_GameManager->prevLevel;
			}

			ImGui::End();

			g_Renderer->Draw(DirectX::Colors::Black);
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
					ImGuiWindowFlags_::ImGuiWindowFlags_NoMove|ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar
					|ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoBackground);

				if (g_GameManager->playerDead)
				{
					if (g_GameManager->playerWon)
					{
						ImGui::Text("You win");

						if (ImGui::Button("Restart", { 100,50 }))
						{
							g_GameManager->LoadLevel(dataJson2, EGameLevel::Level0);
						}
					}
					else
					{
						if (ImGui::Button("Restart", { 100,50 }))
						{
							g_GameManager->LoadLevel(dataJson2, EGameLevel::Level0);
						}
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
			g_Renderer->Draw(DirectX::Colors::Green);

#ifdef _DEBUG
			g_DebugRenderer->Draw(g_Renderer->GetContext(), g_Renderer->GetWidth(), g_Renderer->GetHeight());
#endif
			break;
		}

		g_Renderer->PresentFrame();
	}

	CoUninitialize();

	g_GameObjManager->DeleteAll();
	g_EventManager->Reset();

	SDL_DestroyWindow(g_pWindow);
	SDL_Quit();

	return 0;
}

