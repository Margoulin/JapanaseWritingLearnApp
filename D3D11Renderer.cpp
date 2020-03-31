#include "D3D11Renderer.h"

#include "Window.h"

#include <dxgi1_4.h>
#include <d3d11_3.h>
#include <imgui.h>

auto	D3D11Renderer::Initialize(Window* win) -> void
{
	window = win;
	createDeviceObjects();
	createRenderTarget();

	spriteBatch = new DirectX::SpriteBatch(deviceContext);
	font = new DirectX::SpriteFont(device, L"default.spritefont");

}

auto	D3D11Renderer::Shutdown() -> void
{
	delete spriteBatch;
	delete font;
	shutdownRenderTarget();
	shutdownDeviceObjects();
}

auto	D3D11Renderer::BeginRender() -> void
{
	Vector4F	clearColor(0.5f, 0.5f, 0.5f, 1.0f);
	deviceContext->ClearRenderTargetView(mainRenderTargetView, &(clearColor.x));
	spriteBatch->Begin();
}

auto	D3D11Renderer::EndRender() -> void
{
	spriteBatch->End();
}

auto	D3D11Renderer::createRenderTarget() -> void
{
	DXGI_SWAP_CHAIN_DESC sd;
	swapChain->GetDesc(&sd);

	ID3D11Texture2D* pBackBuffer = nullptr;
	D3D11_RENDER_TARGET_VIEW_DESC render_target_view_desc;
	ZeroMemory(&render_target_view_desc, sizeof(render_target_view_desc));
	render_target_view_desc.Format = sd.BufferDesc.Format;
	render_target_view_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)& pBackBuffer);
	device->CreateRenderTargetView(pBackBuffer, &render_target_view_desc, &mainRenderTargetView);
	deviceContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
	pBackBuffer->Release();

	D3D11_VIEWPORT m_screenViewport = CD3D11_VIEWPORT(
		0.0f, 1.0f, 1280.0f, 720.0f);

	deviceContext->RSSetViewports(1, &m_screenViewport);
}

auto	D3D11Renderer::createDeviceObjects() -> void
{
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Windowed = TRUE;
	sd.OutputWindow = window->GetWindow();
	sd.BufferCount = 2;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.SampleDesc.Count = 1;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlag = 0;
#ifdef _DEBUG
	createDeviceFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL featureLevel;
	D3D_FEATURE_LEVEL featureLevelArray[1] = { D3D_FEATURE_LEVEL_11_0 };
	if (D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlag, 
		featureLevelArray, 1, D3D11_SDK_VERSION, &sd, &swapChain, &device, &featureLevel, &deviceContext) != S_OK)
	{ }
#ifdef _DEBUG
	device->QueryInterface(__uuidof(ID3D11Debug), (void**)(&debugInterface));
#endif // _DEBUG
}

auto	D3D11Renderer::shutdownRenderTarget() -> void
{
	if (mainRenderTargetView)
	{
		mainRenderTargetView->Release();
		mainRenderTargetView = nullptr;
	}
}

auto	D3D11Renderer::shutdownDeviceObjects() -> void
{
	if (swapChain)
	{
		swapChain->Release();
		swapChain = nullptr;
	}
	if (deviceContext)
	{
		deviceContext->Release();
		deviceContext = nullptr;
	}
	if (device)
	{
		device->Release();
		device = nullptr;
	}
#ifdef _DEBUG
	debugInterface->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	if (debugInterface)
		debugInterface->Release();
	debugInterface = nullptr;
#endif // _DEBUG
}

#ifdef _DEBUG

void	D3D11Renderer::MarkD3D11ObjectName(ID3D11DeviceChild* child, MString const& resourceName, MString const& additionnalInfo, MString const& filename)
{
	if (child != nullptr)
	{
		MString total = resourceName + additionnalInfo;
		if (filename.Count() > 0)
			total += MString("  File : ") + filename;
		child->SetPrivateData(WKPDID_D3DDebugObjectName, total.Count(), total.Str());
	}
}

#endif