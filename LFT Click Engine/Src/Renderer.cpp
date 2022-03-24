// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	Graphics.cpp
// Author			:	Vance Howald, Abhijit Zala
// Creation Date	:	2021/11/14
// Purpose			:	DX11 Rendering stuff
// History			:
// 2021/12/01		-	Fixed color space issue
// ---------------------------------------------------------------------------
#include "pch.h"
#include "Renderer.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <WICTextureLoader.h>
#include "Helpers.h"
#include "Components/Drawable.h"
#include "GameObjectManager.h"
#include <CommonStates.h>
#include "GameManager.h"

using namespace Microsoft::WRL;
using namespace DirectX;

namespace
{
#include "Shaders/Compiled/Compiled_PS.h"
#include "Shaders/Compiled/Compiled_VS.h"
}

Renderer::Renderer() :
	width(0), height(0),
	backBufferFormat(DXGI_FORMAT_B8G8R8A8_UNORM),
	depthStencilBufferFormat(DXGI_FORMAT_D24_UNORM_S8_UINT),
	depthStencilViewFormat(DXGI_FORMAT_D24_UNORM_S8_UINT),
	msaaQuality(0),
	msaaSampleCount(4)
{
}

Renderer::~Renderer()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}
void Renderer::Initialize(HWND hWnd, int initWidth, int initHeight)
{
	//setup devices

	UINT flags = 0;

#if defined(DEBUG) || defined(_DEBUG)
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevel;

	HRESULT hr = D3D11CreateDevice(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		flags,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		device.ReleaseAndGetAddressOf(),
		&featureLevel,
		immediateContext.ReleaseAndGetAddressOf());

	if (FAILED(hr))
		assert(false);

	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
		assert(false);


	DXGI_SWAP_CHAIN_DESC sd;

	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = backBufferFormat;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

	DX::ThrowIfFailed(device->CheckMultisampleQualityLevels(backBufferFormat, msaaSampleCount, &msaaQuality));
	assert(msaaQuality > 0);

	sd.SampleDesc.Count = msaaSampleCount;
	sd.SampleDesc.Quality = msaaQuality - 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hWnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	ComPtr<IDXGIDevice> dxgiDevice;
	if FAILED(device.As(&dxgiDevice))
		assert(false);


	ComPtr<IDXGIAdapter> dxgiAdapter;
	if FAILED(dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf()))
		assert(false);


	ComPtr<IDXGIFactory1> dxgiFactory;
	if FAILED(dxgiAdapter->GetParent(__uuidof(IDXGIFactory1), &dxgiFactory))
		assert(false);

	if FAILED(dxgiFactory->CreateSwapChain(device.Get(), &sd, swapChain.ReleaseAndGetAddressOf()))
		assert(false);


	//disable ALT-enter fullscreen
	dxgiFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_WINDOW_CHANGES);

	OnResize(initWidth, initHeight);


	CreateDeviceDependentResources();
}

void Renderer::CreateDeviceDependentResources()
{
	spriteBatch = std::make_unique<SpriteBatch>(immediateContext.Get());
	spriteFont = std::make_unique<SpriteFont>(device.Get(), L"Resources\\fonts\\font.spritefont");
	states = std::make_unique<CommonStates>(device.Get());

	VS_cbPerObjectData.Create(device.Get());
	PS_cbPerObjectData.Create(device.Get());

	VertexType vertices[] =
	{
		XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT2(0.0f, 1.0f),
		XMFLOAT3(-0.5f, 0.5f, 0.0f), XMFLOAT2(0.0f, 0.0f),
		XMFLOAT3(0.5f, 0.5f, 0.0f), XMFLOAT2(1.0f, 0.0f),
		XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT2(1.0f, 1.0f)
	};

	short indices[] =
	{
		0, 1, 2,
		2, 3, 0
	};

	Helpers::CreateMeshBuffer(device.Get(), vertices, sizeof(vertices) / sizeof(vertices[0]), D3D11_BIND_VERTEX_BUFFER, vertBuf.ReleaseAndGetAddressOf());
	Helpers::CreateMeshBuffer(device.Get(), indices, sizeof(indices) / sizeof(indices[0]), D3D11_BIND_INDEX_BUFFER, indexBuf.ReleaseAndGetAddressOf());

	device->CreatePixelShader(g_CompiledPS, sizeof(g_CompiledPS), nullptr, &pixelShader);
	device->CreateVertexShader(g_CompiledVS, sizeof(g_CompiledVS), nullptr, &vertShader);

	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	g_Renderer->GetDevice()->CreateInputLayout(ied, (UINT)std::size(ied), g_CompiledVS, sizeof(g_CompiledVS),
		inputLayout.ReleaseAndGetAddressOf());

}

void Renderer::PrepareForRendering()
{
	immediateContext->ClearRenderTargetView(renderTargetView.Get(), DirectX::Colors::Red);
	immediateContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	spriteBatch->Begin();
}

void Renderer::Draw()
{
	int called = 0;
	int skipped = 0;
	UINT stride = sizeof(VertexType);
	UINT offset = 0;

	immediateContext->IASetVertexBuffers(0, 1, vertBuf.GetAddressOf(), &stride, &offset);
	immediateContext->IASetIndexBuffer(indexBuf.Get(), DXGI_FORMAT_R16_UINT, 0);
	immediateContext->PSSetShader(pixelShader.Get(), nullptr, 0);
	immediateContext->VSSetShader(vertShader.Get(), nullptr, 0);
	immediateContext->IASetInputLayout(inputLayout.Get());
	immediateContext->VSSetConstantBuffers(0, 1, VS_cbPerObjectData.GetAddressOf());
	immediateContext->PSSetConstantBuffers(0, 1, PS_cbPerObjectData.GetAddressOf());
	
	ID3D11SamplerState* samStates[] = { states->PointWrap() };
	immediateContext->PSSetSamplers(0, 1, samStates);
	
	immediateContext->OMSetBlendState(states->AlphaBlend(), nullptr, 0xFFFFFFFFu);
	immediateContext->RSSetState(nullptr);
	immediateContext->OMSetDepthStencilState(nullptr, 0);

	DirectX::XMMATRIX projectionMat = g_GameManager->mainCamera->GetProjectionMatrix();
	auto gameObjectIt = g_GameObjManager->gameObjectList.end();
	while (gameObjectIt != g_GameObjManager->gameObjectList.begin())
	{
		--gameObjectIt;
		GameObject* gameObject = *gameObjectIt;
		Drawable* drawable = gameObject->getComponent<Drawable>();
	
		if (drawable == nullptr)
		{

			continue;
		}
		if (!gameObject->isOnScreen)
		{
			skipped++;
			continue;
		}
	
		//Transform* drawableTransform = gameObject->getComponent<Transform>();
	
		DirectX::XMMATRIX mat = gameObject->getComponent<Transform>()->GetXMMatrix();
	
		const VS_cbPerObject cbValues_VS =
		{
			{
				mat * projectionMat
			},
			XMFLOAT2(drawable->xOffset, drawable->yOffset),
			XMFLOAT2(drawable->xScale, drawable->yScale),
			drawable->xFlip
		};
	
		const PS_cbPerObject cbValues_PS = {
			drawable->alphaOverride
		};
	
		VS_cbPerObjectData.SetData(immediateContext.Get(), cbValues_VS);
		PS_cbPerObjectData.SetData(immediateContext.Get(), cbValues_PS);
	
		immediateContext->PSSetShaderResources(0, 1, drawable->textureSRV.GetAddressOf());
	
		immediateContext->DrawIndexed(6, 0, 0);
		called++;
	}
	int a = 0;
	//for (auto gameObject : g_GameObjManager->gameObjectList)
	//{
	//	Drawable* drawable = gameObject->getComponent<Drawable>();
	//
	//	if (drawable == nullptr || !gameObject->isOnScreen)
	//		continue;
	//
	//	Transform* drawableTransform = gameObject->getComponent<Transform>();
	//
	//	DirectX::XMMATRIX mat = drawableTransform->GetXMMatrix();
	//
	//	const VS_cbPerObject cbValues_VS =
	//	{
	//		{
	//			mat * projectionMat
	//		},
	//		XMFLOAT2(drawable->xOffset, drawable->yOffset),
	//		XMFLOAT2(drawable->xScale, drawable->yScale),
	//		drawable->xFlip
	//	};
	//
	//	const PS_cbPerObject cbValues_PS = {
	//		drawable->alphaOverride
	//	};
	//
	//	VS_cbPerObjectData.SetData(immediateContext.Get(), cbValues_VS);
	//	PS_cbPerObjectData.SetData(immediateContext.Get(), cbValues_PS);
	//	
	//	immediateContext->PSSetShaderResources(0, 1, drawable->textureSRV.GetAddressOf());
	//
	//	immediateContext->DrawIndexed(6, 0, 0);
	//}
}

void Renderer::PresentFrame()
{
	if (ImGui::BeginMainMenuBar())
	{
		ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::EndMainMenuBar();
	}

	spriteBatch->End();
	
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	DX::ThrowIfFailed(swapChain->Present(0, 0));
}


void Renderer::UpdateClientSizeVars()
{
	//TODO -> Get current width/height from resized window
}

void Renderer::InitImGui(SDL_Window* pWindow)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplSDL2_InitForD3D(pWindow);
	ImGui_ImplDX11_Init(device.Get(), immediateContext.Get());
}

void Renderer::OnResize(int newWidth, int newHeight)
{
	this->width = newWidth;
	this->height = newHeight;
	UpdateClientSizeVars();

	renderTargetView.Reset();
	depthStencilView.Reset();
	depthStencilBuffer.Reset();

	DX::ThrowIfFailed(swapChain->ResizeBuffers(1, width, height, backBufferFormat, 0));

	ComPtr<ID3D11Texture2D> backBuffer;
	DX::ThrowIfFailed(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf())));
	DX::ThrowIfFailed(device->CreateRenderTargetView(backBuffer.Get(), 0, renderTargetView.ReleaseAndGetAddressOf()));

	backBuffer.Reset();

	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
	depthStencilDesc.Format = depthStencilBufferFormat;
	depthStencilDesc.SampleDesc.Count = msaaSampleCount;
	depthStencilDesc.SampleDesc.Quality = msaaQuality - 1;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	DX::ThrowIfFailed(device->CreateTexture2D(&depthStencilDesc, 0, depthStencilBuffer.ReleaseAndGetAddressOf()));


	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = CD3D11_DEPTH_STENCIL_VIEW_DESC(
		depthStencilBuffer.Get(),
		msaaSampleCount > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D,
		depthStencilViewFormat);

	DX::ThrowIfFailed(device->CreateDepthStencilView(depthStencilBuffer.Get(), &depthStencilViewDesc, depthStencilView.ReleaseAndGetAddressOf()));

	immediateContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

	screenViewport.TopLeftX = 0.0f;
	screenViewport.TopLeftY = 0.0f;
	screenViewport.MinDepth = 0.0f;
	screenViewport.MaxDepth = 1.0f;
	screenViewport.Width = static_cast<float>(width);
	screenViewport.Height = static_cast<float>(height);

	immediateContext->RSSetViewports(1, &screenViewport);

}


