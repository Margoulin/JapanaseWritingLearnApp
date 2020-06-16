#include "Application.h"

#include "Scene.h"
#include "Window.h"
#include "D3D11Renderer.h"
#include "Database.h"

#include <imgui.h>
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

Application* Application::Instance = nullptr;

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
	
	Instance = this;
}

auto	Application::Shutdown() -> void
{
	Instance = nullptr;
	if (scene)
	{
		scene->Shutdown();
		delete scene;
		scene = nullptr;
	}
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

	if (pendingScene)
	{
		SetScene(pendingScene);
		pendingScene = nullptr;
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (scene)
		scene->Update();

	if (scene)
		scene->ImGuiUpdate();

	renderer->BeginRender();

	if (scene)
		scene->Render(renderer);

	ImGui::Begin("Database");
	ImGui::SetWindowSize({580.0f, 720.0f});
	ImGui::SetWindowPos({700.0f, 0.0f});
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	database->ImGuiUpdate();

	ImGui::End();

	renderer->EndRender();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	renderer->Swap();
}

auto	Application::LoadScene(Scene* newScene) -> void
{
	if (pendingScene)
		return;
	pendingScene = newScene;
}

auto	Application::SetScene(Scene* inScene) -> void
{
	if (scene)
	{
		scene->Shutdown();
		delete scene;
		scene = nullptr;
	}
	scene = inScene;
	inScene->Initialize();
}