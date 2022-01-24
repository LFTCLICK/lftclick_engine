// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	LFTClick Engine.cpp
// Author			:	Vance Howald
// Creation Date	:	2021/10/06
// Purpose			:	implementation of the 'play' game state
// History			: 
// 2021/10/29		-	Added component based arch
// 2021/12/01		-	Added messaging
// ---------------------------------------------------------------------------
#define no_init_all deprecated
#include <iostream>
#include <SDL.h>
#include <SDL_syswm.h>
#include <Windows.h>
#include "InputManager.h"
#include "FrameRateControler.h"
#include "ResourceManager.h"
#include "GameObjectFactory.h"
#include "Components/GameObjectManager.h"
#include <fstream>
#include <json.hpp>
#include "Graphics/Graphics.h"
#include "Components/Transform.h"
#include "EventManager.h"
#include "Messages.h"
#include "GameManager.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_dx11.h"
#include <time.h>
#include "Components/Drawable.h"

using json = nlohmann::json;

int windowWidth = 1000;
int windowHeight = 1000;

GameObjectFactory *gof;
GameObjectManager *gom;

int main(int argc, char *args[])
{
	SDL_Window *pWindow;
	int error = 0;					//temp varrible for the SDL initialization
	
	//Init SDL
	if ((error = SDL_Init(SDL_INIT_VIDEO)) < 0)
	{
		std::cout << "SDL_Init failled, erorr" << error << std::endl;
		return 1;
	}

	//Create SDL window
	pWindow = SDL_CreateWindow("LFT Click Engine Demo",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		windowWidth,
		windowHeight,
		SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
	);
	if (pWindow == NULL) //error check
	{
		std::cout << "Couldn't create window " << SDL_GetError();
		return 1;
	}
	//load in and set icon
	SDL_Surface* icon = ResourceManager::getInstance().GetResource("Resources\\images\\icon.bmp");
	SDL_SetWindowIcon(pWindow, icon);

	//Get handle to SDL window for DirectX
	HWND sdlWindow = GetActiveWindow();
	Graphics::getInstance().init(sdlWindow, windowWidth, windowHeight);

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

	gom = new GameObjectManager();
	EventManager::getInstance().init(gom);
	gof = new GameObjectFactory();
	

	Uint32 frameTimeTicks = 16;		
	bool isRunning = true;


	FrameRateControler::getInstance().Init(6);
	bool masterLoop = true;
	bool playGame = false;
	bool doMenu = true;
	int menuIndex = 0;
	srand(time(NULL));
	while (masterLoop)
	{
		if (doMenu)
		{
			gom->Deserialize(gof, dataJson);

			GameObject* temp = gom->FindObjectOfTag("controls");
			Drawable* controlsGaphics = temp->getComponent<Drawable>();

			gom->FindObjectOfTag("controls")->isActive = false;
			gom->FindObjectOfTag("credits")->isActive = false;


			GameManager::getInstance().gom = gom;
			GameManager::getInstance().mainCamera = gom->FindObjectOfTag("camera")->getComponent<Camera>();
			GameObject* cloneTest = gom->CloneObject(gom->FindObjectOfTag("camera"));

			gom->Start();
			isRunning = true;
			int oldMouseX = 0;
			int oldMouseY = 0;
			while (isRunning)
			{
				FrameRateControler::getInstance().StartOfFrame();
				SDL_Event e;
				while (SDL_PollEvent(&e) != 0)
				{
					ImGui_ImplSDL2_ProcessEvent(&e);
					if (e.type == SDL_QUIT)
					{
						doMenu = false;
						playGame = false;
						isRunning = false;
						masterLoop = false;
					}
					if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_RESIZED)
					{
						int xDelta = windowWidth - e.window.data1;
						int yDelta = windowHeight - e.window.data2;
						int minChange = std::min(xDelta, yDelta);
						windowWidth -= minChange;
						windowHeight -= minChange;
						SDL_SetWindowSize(pWindow, windowWidth, windowHeight);
					}
				}
				Input_Manager::getInstance().Update();

				// Start the Dear ImGui frame
				ImGui_ImplDX11_NewFrame();
				ImGui_ImplSDL2_NewFrame();
				ImGui::NewFrame();
				Graphics::getInstance().ClearBuffer(0x7CA3FF);

				bool open = true;
				if (menuIndex == 0)
				{
					ImGui::SetNextWindowPos({ roundf((windowWidth/2)-50), roundf(windowHeight*.6f) });
					ImGui::Begin("mainMenu", &open, ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize);
					if (ImGui::Button("Play", { 100,50 }))
					{
						isRunning = false;
						playGame = true;
						doMenu = false;
					}
					if (ImGui::Button("Controls", { 100,50 }))
					{
						gom->FindObjectOfTag("controls")->isActive = true;
						gom->FindObjectOfTag("title")->isActive = false;
						menuIndex = 1;
					}
					if (ImGui::Button("Credits", { 100,50 }))
					{
						gom->FindObjectOfTag("credits")->isActive = true;
						gom->FindObjectOfTag("title")->isActive = false;
						menuIndex = 1;
					}
					if (ImGui::Button("Quit", { 100,50 }))
					{
						doMenu = false;
						isRunning = false;
						masterLoop = false;
					}
					
					ImGui::End();
					
					ImGui::SetNextWindowPos({ 0,950 });
					ImGui::Begin("passiveAgressiveMouseInput", &open, ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize);
					std::string toShow = "Could you move your mouse please?";
					if (Input_Manager::getInstance().mouseX() != oldMouseX || Input_Manager::getInstance().mouseY() != oldMouseY)
						toShow = "YAYYYYY I love it when you move your mouse";
					if (Input_Manager::getInstance().isMouseButtonPressed(0))
						toShow = "Left mouse is a good button";
					if (Input_Manager::getInstance().isMouseButtonPressed(1))
						toShow = "I really like right mouse";
					if (Input_Manager::getInstance().isMouseButtonPressed(2))
						toShow = "MIDDLE MOUSE!!!!";
					oldMouseX = Input_Manager::getInstance().mouseX();
					oldMouseY = Input_Manager::getInstance().mouseY();
					ImGui::Text(toShow.c_str());
					ImGui::End();
				}
				else if (menuIndex == 1)
				{
					ImGui::SetNextWindowPos({ roundf((windowWidth / 2) - 50), roundf(windowHeight*.6f) });
					ImGui::Begin("2ndWindow", &open, ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize);

					if (ImGui::Button("Back", { 100,50 }))
					{
						gom->FindObjectOfTag("controls")->isActive = false;
						gom->FindObjectOfTag("credits")->isActive = false;
						gom->FindObjectOfTag("title")->isActive = true;
						menuIndex = 0;
					}
					ImGui::End();

				}


				gom->Draw();
				ImGui::Render();
				ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
				Graphics::getInstance().EndFrame();
				FrameRateControler::getInstance().EndOfFrame();
			}
			gom->DeleteAll();
		}

		if (playGame)
		{

			std::fstream other("./Resources/json/prefabs.json");
			json dataJson2;
			other >> dataJson2;
			other.close();
			gom->Deserialize(gof, dataJson2);
			GameObject* playerObj = gom->FindObjectOfTag("player");

			GameManager::getInstance().mainCamera = gom->FindObjectOfTag("camera")->getComponent<Camera>();
			gom->Start();
			isRunning = true;
			unsigned int lastTime = 0, currentTime;
			FrameRateControler::getInstance().Init(6);//if there has been a considerable gap between EndOfFrame and StartOfFrame call this first so that the first delta time isn't absurdly long
			while (isRunning)
			{
				FrameRateControler::getInstance().StartOfFrame();

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
						int xDelta = windowWidth - e.window.data1;
						int yDelta = windowHeight - e.window.data2;
						int minChange = std::min(xDelta, yDelta);
						windowWidth -= minChange;
						windowHeight -= minChange;
						SDL_SetWindowSize(pWindow, windowWidth, windowHeight);
					}
				}
				Input_Manager::getInstance().Update();
				gom->Update();//update gameobjects
				gom->DoCollision(playerObj);//handle colision with respect to player, this will need to change
				EventManager::getInstance().Update();//process timed events
				Graphics::getInstance().ClearBuffer(0x7CA3FF);//clear screen
				gom->Draw();//do drawing

				bool open = true;//ImGui stuff
				ImGui::SetNextWindowPos({ 0,0});
				ImGui::Begin("2ndWindow", &open, ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoBackground);
				ImGui::Text("Score: %08d", (int)GameManager::getInstance().playerScore);
				ImGui::End();

				if (GameManager::getInstance().playerDead)
				{
					ImGui::SetNextWindowPos({ 450,600 });
					ImGui::Begin("mainMenu", &open, ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize);
					if (ImGui::Button("Restart", { 100,50 }) || GameManager::getInstance().playerRestart)
					{
						isRunning = false;
						playGame = true;
						doMenu = false;
						masterLoop = true;
					}
					if (ImGui::Button("Main Menu", { 100,50 }))
					{
						isRunning = false;
						playGame = false;
						doMenu = true;
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

				ImGui::Render();//ImGui
				ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
				Graphics::getInstance().EndFrame();//present frame
				FrameRateControler::getInstance().EndOfFrame();//sleep

			}
			gom->DeleteAll();
			EventManager::getInstance().Reset();
			GameManager::getInstance().playerDead = false;
			GameManager::getInstance().playerRestart = false;
			GameManager::getInstance().playerScore = 0;
		}
	}
	

	
	// Cleanup
	gom->DeleteAll();
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	SDL_FreeSurface(icon);
	SDL_DestroyWindow(pWindow);

	SDL_Quit();
    return 0;
}