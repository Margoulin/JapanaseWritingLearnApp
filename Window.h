#ifndef __WINDOW_HPP__
#define __WINDOW_HPP__

#include <Windows.h>
#include <functional>
#include <wrl/client.h>
#include "Maths/Vector.hpp"
#include <String.hpp>

class Window
{
public:
	Window() = default;
	Window(const Window&) = delete;
	Window(Window&&) = delete;
	~Window() = default;

	static	auto	CreateWindowInstance(WNDPROC wndProc)->Window*;
	static	std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)>	eventCallback;

	auto	SetWindowTitle(MString const& value) -> void;

	auto	GetWindow() const -> HWND { return winInstance; }
	auto	Shutdown() -> void;

	auto	SetSize(Vector2F const& value) -> void { size = value; }

	auto	GetSize() const -> Vector2F const& { return size; }

	auto	operator = (const Window&)->Window & = delete;
	auto	operator = (Window&&)->Window & = delete;

protected:

private:
	HWND		winInstance;
	WNDCLASS	wc;
	MString		className;
	Vector2F	size;

#ifdef _DEBUG
public:
	auto	ImGuiUpdate() -> void;
	char	newWindowNameBuffer[50];
#endif
};


#endif /*__WINDOW_HPP__*/