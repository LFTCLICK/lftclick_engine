// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	Renderer.cpp
// Author			:	Abhijit Zala
// Creation Date	:	
// Purpose			:	DX11 Rendering stuff
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

extern SDL_Window* g_pWindow;

namespace
{
#include "Shaders/Compiled/Compiled_PS.h"
#include "Shaders/Compiled/Compiled_VS.h"
#include "Shaders/Compiled/CompiledRenderToTexture_PS.h"
}

Renderer::Renderer() :
	clientWidth(0), clientHeight(0),
	backBufferFormat(DXGI_FORMAT_B8G8R8A8_UNORM),
	depthStencilBufferFormat(DXGI_FORMAT_D24_UNORM_S8_UINT),
	depthStencilViewFormat(DXGI_FORMAT_D24_UNORM_S8_UINT),
	msaaQuality(0),
	msaaSampleCount(1),
	displayModes(nullptr),
	numModes(0)
{

}

Renderer::~Renderer()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	delete[] displayModes;
}
void Renderer::Initialize(HWND hWnd, UINT clientWidth, UINT clientHeight)
{
	this->clientWidth = clientWidth;
	this->clientHeight = clientHeight;

	UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED;

#if defined(_DEBUG)
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
#ifdef _DEBUG
	sd.Windowed = TRUE;
#else
	sd.Windowed = FALSE;
#endif
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	ComPtr<IDXGIDevice> dxgiDevice;
	DX::ThrowIfFailed(device.As(&dxgiDevice));

	ComPtr<IDXGIAdapter> dxgiAdapter;
	DX::ThrowIfFailed(dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf()));
	
	ComPtr<IDXGIFactory1> dxgiFactory;
	DX::ThrowIfFailed(dxgiAdapter->GetParent(__uuidof(IDXGIFactory1), &dxgiFactory));

	//Enumerate supported resolutions
	ComPtr<IDXGIOutput> pOutput;
	DX::ThrowIfFailed(dxgiAdapter->EnumOutputs(0, &pOutput));
	DX::ThrowIfFailed(pOutput->GetDisplayModeList(backBufferFormat, 0, &numModes, NULL));
	displayModes = new DXGI_MODE_DESC[numModes];
	DX::ThrowIfFailed(pOutput->GetDisplayModeList(backBufferFormat, 0, &numModes, displayModes));

	printf_s("Querying supported resoultions:\n");

	for (UINT i = 0; i < numModes; ++i)
	{
		printf_s("%dx%d   %.2fHz\n", displayModes[i].Width, displayModes[i].Height,
			static_cast<float>(displayModes[i].RefreshRate.Numerator) / displayModes[i].RefreshRate.Denominator);
	}

#ifndef _DEBUG
	//Choose highest resolution/refresh rate
	clientWidth = displayModes[numModes - 1].Width;
	clientHeight = displayModes[numModes - 1].Height;
	SDL_SetWindowSize(g_pWindow, clientWidth, clientHeight);
#endif

	sd.BufferDesc.Width = clientWidth;
	sd.BufferDesc.Height = clientHeight;
	sd.BufferDesc.RefreshRate = displayModes[numModes - 1].RefreshRate;

	DX::ThrowIfFailed(dxgiFactory->CreateSwapChain(device.Get(), &sd, swapChain.ReleaseAndGetAddressOf()));

	OnResize(clientWidth, clientHeight);

	//disable ALT-enter fullscreen
	//dxgiFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_WINDOW_CHANGES);

	CreateDeviceDependentResources();
}

void Renderer::CreateDeviceDependentResources()
{
	spriteBatch = std::make_unique<SpriteBatch>(immediateContext.Get());
	spriteFont = std::make_unique<SpriteFont>(device.Get(), L"Resources\\fonts\\font.spritefont");
	commonPipelineStates = std::make_unique<CommonStates>(device.Get());

	VS_cbPerObjectData.Create(device.Get());
	PS_cbPerObjectData.Create(device.Get());
	PSRenderToTex_cbPerObjectData.Create(device.Get());

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

	Helpers::CreateMeshBuffer(device.Get(), vertices, sizeof(vertices) / sizeof(vertices[0]), D3D11_BIND_VERTEX_BUFFER, vertexBuffer.ReleaseAndGetAddressOf());
	Helpers::CreateMeshBuffer(device.Get(), indices, sizeof(indices) / sizeof(indices[0]), D3D11_BIND_INDEX_BUFFER, indexBuffer.ReleaseAndGetAddressOf());


	VertexType screenQuadVertices[] =
	{
		XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f),
		XMFLOAT3(-1.0f, +1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f),
		XMFLOAT3(+1.0f, +1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f),
		XMFLOAT3(+1.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f)
	};

	short screenQuadIndices[] =
	{
		0, 1, 2,
		0, 2, 3
	};

	Helpers::CreateMeshBuffer(device.Get(), screenQuadVertices, sizeof(screenQuadVertices) / sizeof(screenQuadVertices[0]), D3D11_BIND_VERTEX_BUFFER, screenQuadVB.ReleaseAndGetAddressOf());
	Helpers::CreateMeshBuffer(device.Get(), screenQuadIndices, sizeof(screenQuadIndices) / sizeof(screenQuadIndices[0]), D3D11_BIND_INDEX_BUFFER, screenQuadIB.ReleaseAndGetAddressOf());
	
	DX::ThrowIfFailed(device->CreatePixelShader(g_CompiledPS, sizeof(g_CompiledPS), nullptr, &pixelShader));
	DX::ThrowIfFailed(device->CreatePixelShader(g_CompiledRenderToTexturePS, sizeof(g_CompiledRenderToTexturePS), nullptr, &renderToTexPixelShader));
	DX::ThrowIfFailed(device->CreateVertexShader(g_CompiledVS, sizeof(g_CompiledVS), nullptr, &vertexShader));

	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	DX::ThrowIfFailed(device->CreateInputLayout(ied, (UINT)std::size(ied), g_CompiledVS, sizeof(g_CompiledVS),
		inputLayout.ReleaseAndGetAddressOf()));

	CD3D11_DEFAULT d3dDefault;
	CD3D11_BLEND_DESC desc(d3dDefault);
	
	desc.AlphaToCoverageEnable = TRUE;
	desc.RenderTarget[0].BlendEnable = TRUE;
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

	DX::ThrowIfFailed(device->CreateBlendState(&desc, alphaToCoverageBS.ReleaseAndGetAddressOf()));

	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFileEx(g_Renderer->GetDevice(),
			L"Resources\\images\\23465-shade.jpg", 0,
			D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0, DirectX::WIC_LOADER_IGNORE_SRGB, nullptr,
			darknessSRV.ReleaseAndGetAddressOf() ) 
	);

}

void Renderer::PrepareForRendering()
{
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	spriteBatch->Begin();
}

void Renderer::Draw(const FLOAT* clearColor)
{
	UINT stride = sizeof(VertexType);
	UINT offset = 0;

	//Bind offscreen texture
	immediateContext->OMSetRenderTargets(1, renderToTextureRTV.GetAddressOf(), depthStencilView.Get());
	immediateContext->ClearRenderTargetView(renderToTextureRTV.Get(), clearColor);
	immediateContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	immediateContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	immediateContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	immediateContext->PSSetShader(pixelShader.Get(), nullptr, 0);
	immediateContext->VSSetShader(vertexShader.Get(), nullptr, 0);
	immediateContext->IASetInputLayout(inputLayout.Get());
	immediateContext->VSSetConstantBuffers(0, 1, VS_cbPerObjectData.GetAddressOf());
	immediateContext->PSSetConstantBuffers(0, 1, PS_cbPerObjectData.GetAddressOf());
	
	ID3D11SamplerState* samStates[] = { commonPipelineStates->PointWrap() };
	immediateContext->PSSetSamplers(0, 1, samStates);
	
	immediateContext->OMSetBlendState(commonPipelineStates->AlphaBlend(), nullptr, 0xFFFFFFFFu);
	immediateContext->RSSetState(nullptr);
	immediateContext->OMSetDepthStencilState(nullptr, 0);

	DirectX::XMMATRIX projectionMat = g_GameManager->mainCamera->GetProjectionMatrix();

	auto gameObjectIt = g_GameObjManager->gameObjectList.end();
	while (gameObjectIt != g_GameObjManager->gameObjectList.begin())
	{
		--gameObjectIt;
		GameObject* gameObject = *gameObjectIt;
		Drawable* drawable = gameObject->getComponent<Drawable>();
	
		if (drawable == nullptr || !gameObject->isOnScreen)
			continue;

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
	
		const PS_cbPerObject cbValues_PS =
		{
			(gameObject->tag == "player" || gameObject->tag == "zombie") ? 0.0f : g_GameManager->displayDarknessLevel
		};
	
		VS_cbPerObjectData.SetData(immediateContext.Get(), cbValues_VS);
		PS_cbPerObjectData.SetData(immediateContext.Get(), cbValues_PS);
	
		ID3D11ShaderResourceView* inputSRVs[] = { drawable->textureSRV.Get(), darknessSRV.Get() };

		immediateContext->PSSetShaderResources(0, 2, inputSRVs);
	
		immediateContext->DrawIndexed(6, 0, 0);
	}

	//Unbind offscreen texture and draw it
	immediateContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());
	immediateContext->ClearRenderTargetView(renderTargetView.Get(), DirectX::Colors::Red);
	immediateContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
	immediateContext->OMSetBlendState(alphaToCoverageBS.Get(), nullptr, 0xFFFFFFFFu);

	immediateContext->IASetVertexBuffers(0, 1, screenQuadVB.GetAddressOf(), &stride, &offset);
	immediateContext->IASetIndexBuffer(screenQuadIB.Get(), DXGI_FORMAT_R16_UINT, 0);

	immediateContext->PSSetShader(renderToTexPixelShader.Get(), nullptr, 0);
	immediateContext->PSSetConstantBuffers(0, 1, PSRenderToTex_cbPerObjectData.GetAddressOf());

	const VS_cbPerObject cbValues_VS =
	{
		XMMatrixIdentity(),
		XMFLOAT2(0, 0),
		XMFLOAT2(1, 1),
		1
	};

	static float f = 0.0f;
	ImGui::DragFloat("Fad factor", &f, 0.01f, 0.0f, 1.0f);

	const PSRenderToTex_cbPerObject cbValues_PS = 
	{
		g_GameManager->darknessLevel,
		g_GameManager->rednessFactor,
		f
	};


	VS_cbPerObjectData.SetData(immediateContext.Get(), cbValues_VS);
	PSRenderToTex_cbPerObjectData.SetData(immediateContext.Get(), cbValues_PS);

	ID3D11ShaderResourceView* inputSRVs[] = { renderToTextureSRV.Get(), darknessSRV.Get() };

	immediateContext->PSSetShaderResources(0, 2, inputSRVs);
	immediateContext->DrawIndexed(6, 0, 0);
}

void Renderer::PresentFrame()
{
	static bool VSync = true;
	if (ImGui::BeginMainMenuBar())
	{
		ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Checkbox("VSync", &VSync);
		ImGui::EndMainMenuBar();
	}

	spriteBatch->End();
	
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	DX::ThrowIfFailed(swapChain->Present(VSync ? 1 : 0, 0));
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
	ImGui::StyleColorsClassic();
	ImGui_ImplSDL2_InitForD3D(pWindow);
	ImGui_ImplDX11_Init(device.Get(), immediateContext.Get());
}

void Renderer::OnResize(int newWidth, int newHeight)
{
	this->clientWidth = newWidth;
	this->clientHeight = newHeight;

	UpdateClientSizeVars();

	renderTargetView.Reset();
	depthStencilView.Reset();
	depthStencilBuffer.Reset();

	DX::ThrowIfFailed(swapChain->ResizeBuffers(1, clientWidth, clientHeight, backBufferFormat, 0));

	ComPtr<ID3D11Texture2D> backBuffer;
	DX::ThrowIfFailed(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf())));
	DX::ThrowIfFailed(device->CreateRenderTargetView(backBuffer.Get(), 0, renderTargetView.ReleaseAndGetAddressOf()));

	backBuffer.Reset();

	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.Width = clientWidth;
	depthStencilDesc.Height = clientHeight;
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


	screenViewport.TopLeftX = 0.0f;
	screenViewport.TopLeftY = 0.0f;
	screenViewport.MinDepth = 0.0f;
	screenViewport.MaxDepth = 1.0f;
	screenViewport.Width = static_cast<float>(clientWidth);
	screenViewport.Height = static_cast<float>(clientHeight);

	immediateContext->RSSetViewports(1, &screenViewport);


	renderToTextureSRV.Reset();
	renderToTextureRTV.Reset();

	D3D11_TEXTURE2D_DESC texDesc;

	texDesc.Width = clientWidth;
	texDesc.Height = clientHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;  
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> offscreenTex;
	DX::ThrowIfFailed(device->CreateTexture2D(&texDesc, 0, &offscreenTex));


	DX::ThrowIfFailed(device->CreateShaderResourceView(offscreenTex.Get(), 0, &renderToTextureSRV));
	DX::ThrowIfFailed(device->CreateRenderTargetView(offscreenTex.Get(), 0, &renderToTextureRTV));


	immediateContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

}


