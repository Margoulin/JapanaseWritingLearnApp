#ifndef __UIRENDERBASE_HPP__
#define __UIRENDERBASE_HPP__

#include <Maths/Vector.hpp>

class D3D11Renderer;

class UIRenderBase
{
public:
	UIRenderBase() = default;
	UIRenderBase(const UIRenderBase&) = delete;
	UIRenderBase(UIRenderBase&&) = delete;
	~UIRenderBase() = default;

	virtual auto	Render(D3D11Renderer* renderer) -> void = 0;

	auto	SetPosition(Vector2F const& inPos) -> void { position = inPos; }
	auto	SetSize(Vector2F const& inSize) -> void { size = inSize; }
	
	auto	GetPosition() const -> Vector2F const& { return position; }
	auto	GetSize() const -> Vector2F const& { return size; }

	auto	operator = (const UIRenderBase&)->UIRenderBase & = delete;
	auto	operator = (UIRenderBase&&)->UIRenderBase & = delete;

protected:

	Vector2F	position;
	Vector2F	size;
private:
};

#endif /*__UIRENDERBASE_HPP__*/#pragma once
