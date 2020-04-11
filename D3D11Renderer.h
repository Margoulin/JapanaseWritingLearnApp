#pragma once

#include <wrl/client.h>
#include <d3d11.h>

#include <Maths/Vector.hpp>
#include <String.hpp>

#include "DirectXTK-master/Inc/SpriteFont.h"

static const Vector4F ColorWhite(1.0f, 1.0f, 1.0f, 1.0f);

using namespace Microsoft::WRL;

class Window;
class SpriteBatch;
class SpriteFont;

class D3D11Renderer
{
public:
	D3D11Renderer() = default;
	D3D11Renderer(const D3D11Renderer&) = delete;
	D3D11Renderer(D3D11Renderer&&) = delete;
	~D3D11Renderer() = default;

	auto	Initialize(Window* win) -> void;
	auto	Shutdown() -> void;

	auto	Swap() -> void { swapChain->Present(0, 0); }

	auto	BeginRender() -> void;
	auto	EndRender() -> void;

	auto	DrawString(const char* text, Vector2F const& position, Vector4F const& color) -> void;
	auto	DrawString(MString const& text, Vector2F const& position, Vector4F const& color) -> void;

	auto	GetDevice() const -> ID3D11Device* { return device; }
	auto	GetDeviceContext() const -> ID3D11DeviceContext* { return deviceContext; }
	auto	GetRenderTarget() const -> ID3D11RenderTargetView* { return mainRenderTargetView; }

	auto	operator = (const D3D11Renderer&)->D3D11Renderer & = delete;
	auto	operator = (D3D11Renderer&&)->D3D11Renderer & = delete;

protected:

private:
	auto	createRenderTarget() -> void;
	auto	createDeviceObjects() -> void;
	auto	shutdownRenderTarget() -> void;
	auto	shutdownDeviceObjects() -> void;

	ID3D11Device*			device = nullptr;
	ID3D11DeviceContext*	deviceContext = nullptr;
	IDXGISwapChain*			swapChain = nullptr;
	ID3D11RenderTargetView*	mainRenderTargetView = nullptr;
	Window*					window = nullptr;

	DirectX::SpriteBatch*	spriteBatch = nullptr;
	DirectX::SpriteFont*	font = nullptr;

#ifdef _DEBUG
public:
	void		MarkD3D11ObjectName(ID3D11DeviceChild* child, MString const& resourceName, MString const& additionnalInfo = "", MString const& filename = "");
	
	ID3D11Debug* debugInterface = nullptr;
#endif // _DEBUG

};
