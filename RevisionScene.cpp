#include "RevisionScene.h"

#include "D3D11Renderer.h"

auto	RevisionScene::Initialize() -> void
{

}

auto	RevisionScene::Shutdown() -> void
{
}

auto	RevisionScene::Update() -> void
{
}

auto	RevisionScene::Render(D3D11Renderer* rend) -> void
{
	rend->DrawString("Revision Scene", Vector2F(550.0f, 100.0f), ColorWhite);
}