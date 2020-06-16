#include "MainMenuScene.h"

#include <iostream>
#include "D3D11Renderer.h"

#include "Application.h"
#include "RevisionScene.h"

auto	MainMenuScene::Initialize() -> void
{
}

auto	MainMenuScene::Shutdown() -> void
{
}

auto	MainMenuScene::Update() -> void
{
}

auto	MainMenuScene::Render(D3D11Renderer* rend) -> void
{
	rend->DrawString("Main Menu", Vector2F(250.0f, 100.0f), ColorWhite, 0.5f);
}

#include <imgui.h>

auto	MainMenuScene::ImGuiUpdate() -> void
{
	ImGui::Begin("Main Menu");
	ImGui::SetWindowSize({700.0f, 200.0f});
	ImGui::SetWindowPos({0.0f, 520.0f});

	ImGui::Spacing();
	if (ImGui::Button("Start Review Session", { 200.0f, 50.0f }))
		Application::Instance->LoadScene(new RevisionScene());
	ImGui::Button("Add Kanji", {200.0f, 50.0f});
	ImGui::Button("Add Word", {200.0f, 50.0f});

	ImGui::End();
}