#ifndef __UIBUTTON_HPP__
#define __UIBUTTON_HPP__

#include <Maths/Vector.hpp>
#include <vector>

class D3D11Renderer;
class UIRenderBase;

enum ButtonState
{
	Unactive,
	Unhovered,
	Hovered,
	Clicked
};

class UIButton
{
public:
	UIButton() = default;
	UIButton(const UIButton&) = delete;
	UIButton(UIButton&&) = delete;
	~UIButton() = default;

	auto	Update(Vector2F const& mousePose) -> void;
	auto	Render(D3D11Renderer* render) -> void;

	auto	AddUIRenderWidget(UIRenderBase* widget) -> void { renderWidgets.push_back(widget); }

	auto	GetWidgetCount() const -> unsigned int { return (unsigned int)renderWidgets.size(); }
	auto	GetRenderWidget(unsigned int idx) const -> UIRenderBase* { return renderWidgets[idx]; }

	auto	operator = (const UIButton&)->UIButton & = delete;
	auto	operator = (UIButton&&)->UIButton & = delete;

protected:

private:
	std::vector<UIRenderBase*>	renderWidgets;
	ButtonState					state = ButtonState::Unactive;
};


#endif /*__UIBUTTON_HPP__*/#pragma once
