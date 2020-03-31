#include "Window.h"

std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)>	Window::eventCallback = nullptr;

auto	Window::CreateWindowInstance(WNDPROC WndProc) -> Window*
{
	Window* window = new Window();
	window->className = MString("Rémi Androuin");
	MString	windowName = MString("DX11");
	window->wc = { CS_CLASSDC, WndProc, 0, 0, GetModuleHandle(nullptr), nullptr, LoadCursor(nullptr, IDC_ARROW), nullptr, nullptr, window->className.Str() };
	RegisterClass(&(window->wc));

	const int app_width = 1280;
	const int app_height = 720;

	window->size = Vector2F((float)app_width, (float)app_height);

	DWORD style = WS_OVERLAPPEDWINDOW;
	RECT canvasRect = { 0, 0, app_width, app_height };
	AdjustWindowRect(&canvasRect, style, FALSE);

	window->winInstance = CreateWindow(window->className.Str(), windowName.Str(), style, (GetSystemMetrics(SM_CXSCREEN) - app_width) / 2, (GetSystemMetrics(SM_CYSCREEN) - app_height) / 2
		, canvasRect.right - canvasRect.left, canvasRect.bottom - canvasRect.top, nullptr, nullptr, window->wc.hInstance, nullptr);

	if (window->winInstance == nullptr)
	{
		window->Shutdown();
		return nullptr;
	}
	return window;
}

auto	Window::SetWindowTitle(MString const& value) -> void
{
	SetWindowTextA(winInstance, value.Str());
}

auto	Window::Shutdown() -> void
{
	UnregisterClass(className.Str(), wc.hInstance);
}

#include <imgui.h>
#ifdef _DEBUG
auto	Window::ImGuiUpdate() -> void
{
	ImGui::InputFloat2("Size", &size.x, -1, ImGuiInputTextFlags_ReadOnly);

	ImGui::InputText("Window Name", newWindowNameBuffer, 50);
	if (ImGui::Button("Change Name"))
	{
		SetWindowTextA(winInstance, newWindowNameBuffer);
	}
}
#endif // _DEBUG
