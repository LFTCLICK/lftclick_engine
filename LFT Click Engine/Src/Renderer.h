// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	Renderer.h
// Author			:	Vance Howald, Abhijit Zala
// Creation Date	:	2021/11/14
// Purpose			:	DX11 Rendering stuff
// History			:
// ---------------------------------------------------------------------------
#pragma once
#include "Windows.h"
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include <vector>
#include <SpriteBatch.h>
#include <SpriteFont.h>

class Renderer
{
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediateContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;

	DXGI_FORMAT backBufferFormat;
	DXGI_FORMAT depthStencilBufferFormat;
	DXGI_FORMAT depthStencilViewFormat;

	D3D11_VIEWPORT screenViewport;

	UINT msaaQuality;
	UINT msaaSampleCount;

	int width, height;

	std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
	std::unique_ptr<DirectX::SpriteFont> spriteFont;

public:
	Renderer();
	~Renderer();
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;

	void Initialize(HWND hWnd, int initWidth, int initHeight);
	void InitImGui(SDL_Window* pWindow);

	void PrepareForRendering();
	void PresentFrame();
	void OnResize(int newWidth, int newHeight);

	ID3D11DeviceContext* GetContext() const { return immediateContext.Get(); }
	ID3D11Device* GetDevice() const { return device.Get(); }
	DirectX::SpriteBatch* GetSpriteBatch() const { return spriteBatch.get(); }
	DirectX::SpriteFont* GetSpriteFont() const { return spriteFont.get(); }
	int GetWidth() const { return width; }
	int GetHeight() const { return height; }
private:
	void UpdateClientSizeVars();
};

extern std::unique_ptr<Renderer> g_Renderer;
