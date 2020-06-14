#ifndef __MAINMENUSCENE_HPP__
#define __MAINMENUSCENE_HPP__

#include "Scene.h"

class MainMenuScene : public Scene
{
public:
	MainMenuScene() = default;
	MainMenuScene(const MainMenuScene&) = delete;
	MainMenuScene(MainMenuScene&&) = delete;
	~MainMenuScene() = default;

	virtual auto Initialize() -> void;
	virtual auto Shutdown() -> void;

	virtual auto	Update() -> void;
	virtual auto	Render(D3D11Renderer*) -> void;
	
	virtual auto	ImGuiUpdate() -> void;

	auto	operator = (const MainMenuScene&)->MainMenuScene & = delete;
	auto	operator = (MainMenuScene&&)->MainMenuScene & = delete;

protected:

private:

};


#endif /*__MAINMENUSCENE_HPP__*/