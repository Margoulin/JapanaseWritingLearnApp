#include "UITextRender.h"

#include "D3D11Renderer.h"

auto	UITextRender::Render(D3D11Renderer* renderer) -> void
{
	renderer->DrawString(text, position, color);
}