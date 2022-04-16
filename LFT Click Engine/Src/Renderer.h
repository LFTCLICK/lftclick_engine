// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	Renderer.cpp
// Author			:	Abhijit Zala 
// Creation Date	:	2022/03/14
// Purpose			:	DX11 Rendering stuff
// ---------------------------------------------------------------------------
#pragma once
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include "ConstantBuffer.h"
#include "CommonStates.h"
#include "Components/Camera.h"

class Renderer
{
public:
	bool disableDarkness;
	bool fullScreenMode;
private:

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediateContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11BlendState> alphaToCoverageBS;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> renderToTextureSRV;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderToTextureRTV;

	Microsoft::WRL::ComPtr<ID3D11Buffer> screenQuadVB;
	Microsoft::WRL::ComPtr<ID3D11Buffer> screenQuadIB;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> darknessSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> darkness2SRV;

	DXGI_FORMAT backBufferFormat;
	DXGI_FORMAT depthStencilBufferFormat;
	DXGI_FORMAT depthStencilViewFormat;

	D3D11_VIEWPORT screenViewport;

	UINT msaaQuality;
	UINT msaaSampleCount;

	int clientWidth, clientHeight;

	std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
	std::unique_ptr<DirectX::SpriteFont> spriteFont;

	struct  VS_cbPerObject
	{
		DirectX::XMMATRIX transform;
		DirectX::XMFLOAT2 Offset;
		DirectX::XMFLOAT2 Scale;

		float flipX;
	};
	static_assert(sizeof(VS_cbPerObject) % 16 == 0, "Not 16-bytes aligned");

	struct PS_cbPerObject
	{
		float darknessFactor;
		DirectX::XMFLOAT3 padding;
	};
	static_assert(sizeof(PS_cbPerObject) % 16 == 0, "Not 16-bytes aligned");

	struct PSRenderToTex_cbPerObject
	{
		float darknessFactor;
		float rednessFactor;
		float fadeFactor;
		float darknessTextureSwitchFactor;
	};
	static_assert(sizeof(PSRenderToTex_cbPerObject) % 16 == 0, "Not 16-bytes aligned");

	struct VertexType
	{
		DirectX::XMFLOAT3 Pos;
		DirectX::XMFLOAT2 TexCoord;
	};

	ConstantBuffer<VS_cbPerObject> VS_cbPerObjectData;
	ConstantBuffer<PS_cbPerObject> PS_cbPerObjectData;
	ConstantBuffer<PSRenderToTex_cbPerObject> PSRenderToTex_cbPerObjectData;

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> renderToTexPixelShader;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	std::unique_ptr<DirectX::CommonStates> commonPipelineStates;


	DXGI_MODE_DESC* displayModes;
	UINT numModes;
public:
	Renderer();
	~Renderer();
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;
	
	void ToggleFullScreen(SDL_Window* pWindow);
	void Initialize(HWND hWnd, UINT clientWidth, UINT clientHeight);
	void InitImGui(SDL_Window* pWindow);
	void PrepareForRendering();
	void Draw(const FLOAT* clearColor);
	void PresentFrame();
	void OnResize(int newWidth, int newHeight);

	DXGI_MODE_DESC GetSupportedDisplayMode(UINT index) const { return displayModes[index]; }
	UINT GetNumDisplayModes() const { return numModes; }

	ID3D11DeviceContext* GetContext() const { return immediateContext.Get(); }
	ID3D11Device* GetDevice() const { return device.Get(); }
	DirectX::SpriteBatch* GetSpriteBatch() const { return spriteBatch.get(); }
	DirectX::SpriteFont* GetSpriteFont() const { return spriteFont.get(); }
	int GetWidth() const { return clientWidth; }
	int GetHeight() const { return clientHeight; }
	bool IsFullScreen() const { return fullScreenMode; }
private:
	void UpdateClientSizeVars();
	void CreateDeviceDependentResources();
};

extern std::unique_ptr<Renderer> g_Renderer;
