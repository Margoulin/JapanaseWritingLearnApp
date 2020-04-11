#ifndef __UITEXTRENDER_HPP__
#define __UITEXTRENDER_HPP__

#include "UIRenderBase.h"
#include <String.hpp>

class UITextRender : public UIRenderBase
{
public:
	UITextRender() = default;
	UITextRender(const UITextRender&) = delete;
	UITextRender(UITextRender&&) = delete;
	~UITextRender() = default;

	virtual auto	Render(D3D11Renderer* renderer) -> void;

	auto	SetText(MString const& inText) -> void { text = inText; }
	auto	SetColor(Vector4F const& inColor) -> void { color = inColor; }

	auto	GetText() const -> MString const& { return text; }
	auto	GetColor() const -> Vector4F const& { return color; }

	auto	operator = (const UITextRender&)->UITextRender & = delete;
	auto	operator = (UITextRender&&)->UITextRender & = delete;

protected:

private:
	Vector4F	color;
	MString		text;
};


#endif /*__UITEXTRENDER_HPP__*/#pragma once
