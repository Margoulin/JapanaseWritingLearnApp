#ifndef __SCENE_HPP__
#define __SCENE_HPP__

class D3D11Renderer;

class Scene
{
public:
	Scene() = default;
	Scene(const Scene&) = delete;
	Scene(Scene&&) = delete;
	~Scene() = default;

	virtual auto Initialize() -> void = 0;
	virtual auto Shutdown() -> void = 0;

	virtual auto	Update() -> void = 0;
	virtual auto	Render(D3D11Renderer*) -> void = 0;

	virtual auto	ImGuiUpdate() -> void = 0;

	auto	operator = (const Scene&)->Scene & = delete;
	auto	operator = (Scene&&)->Scene & = delete;

protected:

private:

};


#endif /*__SCENE_HPP__*/