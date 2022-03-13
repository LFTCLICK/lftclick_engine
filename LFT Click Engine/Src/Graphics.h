// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	Graphics.h
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
#include "DebugRenderer.h"

class Graphics
{
public:
	Graphics();
	~Graphics() = default;
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;

	void Initialize(HWND hWnd, int initWidth, int initHeight);
	void PrepareForRendering();
	void PresentFrame();

	void Draw();
	int GetWidth();
	int GetHeight();
	ID3D11DeviceContext* GetContext();
	ID3D11Device* GetDevice();
	void OnResize(int newWidth, int newHeight);

private:
	void UpdateClientSizeVars();

private:
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediateContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	int width, height;

	DXGI_FORMAT backBufferFormat;
	DXGI_FORMAT depthStencilBufferFormat;
	DXGI_FORMAT depthStencilViewFormat;

	D3D11_VIEWPORT screenViewport;

	UINT msaaQuality;
	UINT msaaSampleCount;

};

extern std::unique_ptr<Graphics> g_Renderer;
