#include "Application.h"

#include "Window.h"
#include "D3D11Renderer.h"
#include "Database.h"

#include <imgui.h>
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (Window::eventCallback)
		Window::eventCallback(hWnd, msg, wParam, lParam);
	ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);

	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

auto	Application::Initialize() -> void
{
	window = Window::CreateWindowInstance(WndProc);
	ShowWindow(window->GetWindow(), SW_SHOWDEFAULT);
	UpdateWindow(window->GetWindow());

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	renderer = new D3D11Renderer();
	renderer->Initialize(window);

	ImGui_ImplWin32_Init(window->GetWindow());
	ImGui_ImplDX11_Init(renderer->GetDevice(), renderer->GetDeviceContext());
	io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msgothic.ttc", 15.0f, NULL, io.Fonts->GetGlyphRangesJapanese());

	database = new Database();
	database->OpenDatabaseFile("Database.db");
	database->LoadKanji();

	io.WantCaptureMouse = true;
	msg = new tagMSG();
	ZeroMemory(msg, sizeof(msg));
	running = true;
}

auto	Application::Shutdown() -> void
{
	AnimateWindow(window->GetWindow(), 200, AW_HIDE | AW_BLEND);
	database->CloseDatabase();

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	renderer->Shutdown();
	delete renderer;
	window->Shutdown();
	delete window;
}

auto	Application::Update() -> void
{
	ImGuiIO& io = ImGui::GetIO();
	if (msg->message == WM_QUIT)
		running = false;
	if (msg->message == WM_LBUTTONDOWN)
		io.MouseClicked[0] = true;

	if (PeekMessage(msg, nullptr, 0U, 0U, PM_REMOVE))
	{
		TranslateMessage(msg);
		DispatchMessage(msg);
		return;
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	renderer->BeginRender();

	ImGui::Begin("Database");
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	database->ImGuiUpdate();

	ImGui::End();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	renderer->EndRender();
	renderer->Swap();
}