#include "UIButton.h"

#include "UIRenderBase.h"
	
auto	Update(Vector2F const& mousePose) -> void
{

}

auto	UIButton::Render(D3D11Renderer* renderer) -> void
{
	for (auto& widget : renderWidgets)
		widget->Render(renderer);
}