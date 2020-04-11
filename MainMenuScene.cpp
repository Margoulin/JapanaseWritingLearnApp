#include "MainMenuScene.h"

#include <iostream>
#include "D3D11Renderer.h"

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
	rend->DrawString("Main Menu", Vector2F(550.0f, 100.0f), ColorWhite);
	rend->DrawString("Review Session", Vector2F(500.0f, 300.0f), ColorWhite);
}